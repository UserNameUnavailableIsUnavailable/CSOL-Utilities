local Scheduler = {}

local native = require("executor.core.Native")
local Task = require("executor.core.Task")

local suspended = {}
local tasks = {}
local ready = {}
local queued = {}
local current = nil
local running = false

function Scheduler.get_running()
    return current
end

function Scheduler.prepare(task)
    assert(type(task) == "table" and type(task.get_id) == "function"
        and type(task._coroutine) == "thread", "Scheduler.Prepare expects a Task")
    assert(not task:is_done(), "cannot schedule a completed task")
    local id = task:get_id()
    if tasks[id] then
        assert(tasks[id] == task, "duplicate task id")
        return
    end
    tasks[id] = task
    suspended[id] = task
    task._scheduled = true
    task._state = Task.State.kSuspended
end

function Scheduler.submit(id)
    if queued[id] then
        return
    end
    local task = suspended[id]
    assert(task, "Task with id " .. tostring(id) .. " not found in suspended tasks")
    assert(task._state == Task.State.kSuspended, "cannot submit a running task")
    assert(not task._dependency or task._dependency:is_done(),
        "cannot submit a task before its dependency completes")
    assert(not task._waiting_io, "cannot submit a task before its I/O completes")
    ready[#ready + 1] = task
    queued[id] = true
    suspended[id] = nil
    task._state = Task.State.kReady
end

function Scheduler.has_pending()
    return next(tasks) ~= nil
end

local function finish(task, success, result)
    local id = task:get_id()
    tasks[id] = nil
    suspended[id] = nil
    queued[id] = nil
    task._scheduled = false
    task._state = success and Task.State.kDone or Task.State.kFailed
    if success then
        task._result = result
    else
        local closed, close_error = coroutine.close(task._coroutine)
        if not closed and not tostring(result):find(tostring(close_error), 1, true) then
            result = tostring(result) .. "\nCoroutine close: " .. tostring(close_error)
        end
        task._error = result
    end
    local observed = #task._waiters > 0
    for _, waiter in ipairs(task._waiters) do
        Scheduler.submit(waiter:get_id())
    end
    task._waiters = {}
    return observed
end

local function fail(task, message, failures)
    if not finish(task, false, message) then
        failures[#failures + 1] = tostring(task._error)
    end
end

function Scheduler.shutdown()
    assert(not running, "cannot shut down a running scheduler")
    local failures = {}
    for _, task in pairs(tasks) do
        local ok, message = coroutine.close(task._coroutine)
        if not ok then
            failures[#failures + 1] = tostring(message)
        end
        task._state = Task.State.kFailed
        task._error = "scheduler shut down"
        task._scheduled = false
        task._dependency = nil
        task._waiting_io = nil
        task._waiters = {}
    end
    tasks, suspended, ready, queued = {}, {}, {}, {}
    native.Runtime_close()
    if #failures > 0 then
        error(table.concat(failures, "\n"), 0)
    end
end

local function runBatch()
    if not Scheduler.has_pending() then
        return false
    end
    local completions = native.Runtime_poll(#ready == 0 and -1 or 0)
    for _, id in ipairs(completions) do
        local task = suspended[id]
        assert(task and task._waiting_io, "completion for a task without pending I/O")
        task._waiting_io = nil
        Scheduler.submit(id)
    end
    assert(#ready > 0, "scheduler stalled: suspended tasks have no pending I/O")
    local batch = ready
    ready = {}
    local failures = {}
    for _, task in ipairs(batch) do
        local id = task:get_id()
        queued[id] = nil
        suspended[id] = task
        current = task
        task._started = true
        task._state = Task.State.kRunning
        local result = table.pack(coroutine.resume(task._coroutine))
        current = nil
        if not result[1] then
            local message = debug.traceback(task._coroutine, tostring(result[2]))
            fail(task, message, failures)
        elseif coroutine.status(task._coroutine) == "dead" then
            finish(task, true, table.pack(table.unpack(result, 2, result.n)))
        else
            task._state = Task.State.kSuspended
            local request = result[2]
            if result.n ~= 2 or type(request) ~= "table" then
                fail(task, "tasks must yield through Await or Runtime.Sleep", failures)
            elseif request.kind == "sleep" then
                local registered, message = pcall(native.Runtime_sleep, id, request.delay)
                if registered then
                    task._waiting_io = true
                else
                    fail(task, message, failures)
                end
            elseif request.kind ~= "await" or task._dependency ~= request.task then
                fail(task, "unsupported task yield", failures)
            end
        end
    end
    if #failures > 0 then
        error(table.concat(failures, "\n"), 0)
    end
    return Scheduler.has_pending()
end

---Run one ready batch, blocking on IOCP only when no tasks are ready.
---@return boolean pending whether more scheduled tasks remain
function Scheduler.run()
    assert(not running, "Scheduler.Run is not reentrant")
    running = true
    local ok, result = xpcall(runBatch, debug.traceback)
    current = nil
    running = false
    if not ok then
        local closed, cleanup = pcall(Scheduler.shutdown)
        error(closed and result or (tostring(result) .. "\nCleanup: " .. tostring(cleanup)), 0)
    end
    return result
end

---Start a fresh native runtime after explicit Shutdown.
function Scheduler.initialize()
    assert(not running and not Scheduler.has_pending(),
        "cannot initialize a scheduler with pending tasks")
    native.Runtime_open()
end

return Scheduler
