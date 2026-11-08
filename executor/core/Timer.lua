local Timer = {}

local Promise = require("executor.core.Promise")

function Timer:sleep_for(ms)
    local promise = Promise.create(function (resolve, reject)
    end)
    return promise
end

function Timer:sleep_until(tp)
end

return Timer
