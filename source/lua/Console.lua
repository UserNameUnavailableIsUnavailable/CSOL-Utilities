if (not Console)
then
Console = {}
---在控制台上打印字符串，不换行
---@param msg any
---@return nil
function Console:print(msg)
    OutputLogMessage(msg)
end

---在控制台上打印字符串，并换行
---@param msg any
---@return nil
function Console:println(msg)
    OutputLogMessage(msg .. '\n')
end

---在控制台上按照固定格式打印信息
---@param msg any
---@return nil
function Console:infomation(msg)
    self:println("[INFO] " .. msg)
end

---在控制台上按照固定格式打印错误信息
---@param msg any
---@return nil
function Console:error(msg)
    self:println("[ERR] " .. msg)
end

---清空控制台
---@return nil
function Console:clear()
    ClearLog()
end

end -- if (not Console)
