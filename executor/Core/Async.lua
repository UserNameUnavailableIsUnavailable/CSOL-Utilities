local Async = {}

local Runtime = require("executor.Core.Runtime")
local Task = require("executor.Core.Task")
local Scheduler = require("executor.Core.Scheduler")

---Spawn an async task
---@param fn fun(): any
---@return executor.Core.Task
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