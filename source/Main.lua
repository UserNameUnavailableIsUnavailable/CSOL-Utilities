PATH = "C:/Users/Silve/Develop/CSOL-24H/source/Executor/"
---加载 LUA 源文件。
---@param file_name string 文件名（相对路径）。
---@return nil
function Include(file_name)
    if (type(file_name) == "string")
    then
        dofile(PATH .. file_name)
    end
end
Include("Global.lua")
Start()