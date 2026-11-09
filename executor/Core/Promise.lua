---@class Promise
local Promise = {}

local Scheduler = require("executor.core.Scheduler")

---@enum Promise.State
Promise.State = {
    PENDING = 0,
    RESOLVED = 1,
    REJECTED = 2
}

Promise._state = Promise.State.PENDING
Promise._result = nil
Promise._reason = nil
Promise._scheduler = Scheduler
Promise._resolve_callbacks = {}
Promise._reject_callbacks = {}

---Resolve a promise.
---@param result any
function Promise:_resolve(result)
    if self._state ~= Promise.State.PENDING then return end
    self._state = Promise.State.RESOLVED
    self._result = result
    for _, callback in ipairs(self._resolve_callbacks) do
        self._scheduler:submit(function ()
            callback(result)
        end)
    end
end

---Reject a promise.
---@param reason any
function Promise:_reject(reason)
    if self._state ~= Promise.State.PENDING then return end
    self._state = Promise.State.REJECTED
    self._reason = reason
    for _, callback in ipairs(self._reject_callbacks) do
        self._scheduler:submit(function ()
            callback(reason)
        end)
    end
end

---Creates a promise.
---@param fn fun(resolve: fun(result), reject: fun(reason))
---@return Promise
function Promise.create(fn)
    --[[@type Promise]]
    local promise = {
        _state = Promise.State.PENDING,
        _fn = fn,
        _resolve_callbacks = {},
        _reject_callbacks = {},
    }
    setmetatable(promise, { __index = Promise })
    local settled = false
    local function resolve(result)
        if settled then return end
        settled = true
        promise:_resolve(result)
    end
    local function reject(reason)
        if settled then return end
        settled = true
        promise:_reject(reason)
    end
    local ok, reason = pcall(fn, resolve, reject)
    if not ok then
        reject(reason)
    end
    return promise
end

---Create a promise.
---@param fn fun(...))
---@return fun(...): Promise
function Promise.async(fn)
    return function(...)
        local args = table.pack(...)
        local wrapper = function(resolve, reject)
            local co = coroutine.create(function ()
                local ok, r = pcall(fn, table.unpack(args))
                if ok then resolve(r) else reject(r) end
            end)
            -- eager
            local ok, e = coroutine.resume(co)
            if not ok then reject(e) end
        end
        return Promise.create(wrapper)
    end
end

---Wait for results of a promise.
---@param self Promise
---@return any
function Promise:await()
    local current, is_main = coroutine.running()
    -- NOTE: Main corotuine is not yieldable, and if we decide to block wait for the promise in main, then the entire program may stuck. So we enforce the user to await in a non-blocking environment.
    -- For resolved / rejected promises, we also enforce this rule to avoid potential errors.
    if (not current or is_main) then
        error("`Promise.await` is only allowed in async functions")
    end
    if self._state == Promise.State.RESOLVED then
        return self._result
    elseif self._state == Promise.State.REJECTED then
        error(self._reason)
    else -- register the current task to scheduler
        local settled = false
        self:and_then(function (result)
            if settled then return end
            settled = true
            self._scheduler:submit(function ()
                coroutine.resume(current, true, result)
            end)
        end)
        self:or_else(function (reason)
            if settled then return end
            settled = true
            coroutine.resume(current, false, reason)
        end)
        local ok, result_or_reason = coroutine.yield()
        if ok then
            return result_or_reason
        else
            error(result_or_reason)
        end
    end
end

function Promise:get_state()
    return self._state
end

function Promise:set_state(state)
    self._state = state
end

Promise._on_resolve = nil -- called when the promise is resolved

---Chain function call on resolve.
---@param fn fun(result: any)
---@return Promise
function Promise:and_then(fn)
    local parent = self
    local child = Promise.create(function(resolve, reject)
        parent:_on_resolve(function(result)
            local ok, r = pcall(fn, result)
            if ok then
                resolve(r)
            else
                reject(r)
            end
        end)
    end)
    return child
end

---Register a resolve callback.
---@param fn fun(result: any)
function Promise:_on_resolve(fn)
    self._resolve_callbacks[#self._resolve_callbacks+1] = fn
end

---Register a reject callback.
---@param fn fun(reason: any)
function Promise:_on_reject(fn)
    self._reject_callbacks[#self._reject_callbacks+1] = fn
end

---Chain function call on reject.
---@param fn fun(reason: any)
---@return Promise
function Promise:or_else(fn)
    local parent = self
    local child = Promise.create(function(resolve, reject)
        parent:_on_reject(function(reason)
            local ok, r = pcall(fn, reason)
            if ok then
                resolve(r)
            else
                reject(r)
            end
        end)
    end)
    return child
end

---Chain function call on finally.
---@param fn fun()
function Promise:finally(fn)
    local parent = self
    local child = Promise.create(function(resolve, reject)
        parent:_on_resolve(function(result)
            fn() -- if throws, the child promise will reject
            resolve(result)
        end)
        parent:_on_reject(function(reason)
            fn() -- if throws, the child promise will reject
            reject(reason)
        end)
    end)
    return child
end

---Attempts to close a promise early, causing observers to get rejected.
function Promise:close()
    -- TODO: notify the backend to cancel
    self:_reject("Promise closed")
end

return Promise
