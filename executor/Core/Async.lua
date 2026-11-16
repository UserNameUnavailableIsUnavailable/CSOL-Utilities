local Async = {}

local Runtime = require("executor.core.Runtime")
local Task = require("executor.core.Task")
local Scheduler = require("executor.core.Scheduler")

---Spawn an async task
---@param fn fun(): any
---@return executor.core.Task
function Async.spawn(fn)
    local task = Task.create(fn)
    Scheduler.prepare(task)
    return task
end

---Create and run a main task.
---@param fn fun(): nil
function Async.main(fn)
    local task = Task.create(fn)
    Scheduler.prepare(task)
    Scheduler.submit(task:get_id())
    while Scheduler.has_pending() do
        Scheduler.run()
    end
end

return Async