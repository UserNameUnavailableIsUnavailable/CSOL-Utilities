PATH = [[C:/Users/Silver/develop/CSOL-Utilities/Executor/%s]] -- Executor 模块搜索路径
---包含指定模块。
---@param file_name string 模块文件名称
function Include(file_name)
    dofile(PATH:format(file_name))
end
Include("Main.lua") -- 加载入口函数
Main()
