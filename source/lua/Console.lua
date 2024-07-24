if (not Console_lua)
then
Console_lua = true
Console = {}
---在控制台上打印字符串，不换行
---@param fmt string
---@param ... any
---@return nil
function Console:print(fmt, ...)
    OutputLogMessage(fmt, ...)
end

---在控制台上打印字符串，并换行
---@param fmt string
---@param ... any
---@return nil
function Console:println(fmt, ...)
    OutputLogMessage(fmt .. '\n', ...)
end

---在控制台上按照固定格式打印信息
---@param fmt string 
---@param ... any
---@return nil
function Console:infomation(fmt, ...)
    self:println("【信息】" .. fmt, ...)
end

---在控制台上按照固定格式打印错误信息
---@param fmt string
---@return nil
function Console:error(fmt, ...)
    self:println("【错误】" .. fmt, ...)
end

---清空控制台
---@return nil
function Console:clear()
    ClearLog()
end

end -- Console_lua
