local Util = {}

---Strip reserved members starting with an underscore.
---@param t table
---@return table
function Util.strip_reserved_members(t)
    for k in pairs(t) do
        if type(k) == "string" and k:sub(1, 1) == "_" then
            t[k] = nil
        end
    end
    return t
end

return Util