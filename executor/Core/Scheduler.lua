---@class Scheduler
local Scheduler = {}

Scheduler.__index = Scheduler

-- TODO: Use linked list for better performance
Scheduler._suspended = {}
Scheduler._ready = {}
Scheduler._running = nil

function Scheduler:run()
end

---Submit a task.
---@param fn fun()
---@return nil
function Scheduler:submit(fn)
    self._ready[#self._ready + 1] = fn
end

return Scheduler