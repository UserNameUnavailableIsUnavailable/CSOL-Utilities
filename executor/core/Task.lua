---@class executor.core.Task
local Task = {}

Task.State = {
    kReady = 1,
    kRunning = 2,
    kDone = 3,
    kCancelled = 4,
    kFailed = 5,
    kSuspended = 6,
}

Task._id = 0
Task._state = Task.State.kReady
Task._coroutine = nil
Task._waiters = {}
Task._dependency = nil
Task._scheduled = false
Task._started = false
Task._result = { n = 0 }
Task._error = nil

local counter = 0

function Task.create(fn)
    assert(type(fn) == "function", "Task.Create expects a function")
    local task = setmetatable({
        _id = counter,
        _state = Task.State.kReady,
        _coroutine = coroutine.create(fn),
        _waiters = {},
    }, { __index = Task })
    counter = counter + 1
    return task
end

function Task:is_done()
    return self._state == Task.State.kDone or self._state == Task.State.kFailed
        or self._state == Task.State.kCancelled
end

function Task:is_cancelled()
    return self._state == Task.State.kCancelled
end

function Task:get_id()
    return self._id
end

function Task:await()
    if not self:is_done() then
        local Scheduler = require("executor.core.Scheduler")
        local caller = Scheduler.get_running()
        assert(caller and coroutine.running() == caller._coroutine,
            "Task:Await must run inside a scheduled task")
        local dependency = self
        while dependency do
            assert(dependency ~= caller, "cyclic task await")
            dependency = dependency._dependency
        end
        caller._dependency = self
        self._waiters[#self._waiters + 1] = caller
        if not self._scheduled then
            Scheduler.prepare(self)
        end
        if not self._started then
            Scheduler.submit(self:get_id())
        end
        coroutine.yield({ kind = "await", task = self })
        caller._dependency = nil
        assert(self:is_done(), "awaited task resumed before completion")
    end
    if self._state == Task.State.kFailed then
        error(self._error, 0)
    elseif self:is_cancelled() then
        error("task was cancelled", 0)
    end
    return table.unpack(self._result, 1, self._result.n)
end

return Task
