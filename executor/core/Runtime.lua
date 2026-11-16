local Runtime = {}

local Task = require("executor.core.Task")

---Create a lazy sleep task. Its timer starts when the task is scheduled.
---@param milliseconds integer
---@nodiscard
function Runtime.sleep(milliseconds)
    assert(type(milliseconds) == "number" and math.type(milliseconds) == "integer"
        and milliseconds >= 0 and milliseconds <= 2147483647,
        "Sleep expects integer milliseconds from 0 to 2147483647")
    return Task.create(function()
        coroutine.yield({ kind = "sleep", delay = milliseconds })
    end)
end

return Runtime
