# 注意：对于 Powershell 5.1，此文件必须保存为 UTF-8 BOM，否则字符串将默认以 ANSI 进行编码
# 获取路径，反斜线替换为正斜线
$PROJECT_PATH = (Get-Item $PSScriptRoot\Executor\).FullName -replace("\\", "/")
if (-not $PROJECT_PATH.EndsWith("/"))
{
    $PROJECT_PATH += "/"
}
$PROJECT_PATH += "%s"
$code = @"
PATH = [[{0}]] -- Executor 模块搜索路径
---包含指定模块。
---@param file_name string 模块文件名称
function Include(file_name)
    dofile(PATH:format(file_name))
end
Include("Main.lua") -- 加载入口函数
Main()
"@
$file = New-Item -Type File -Path "$PSScriptRoot\Executor.lua" -Force # 创建 Executor.lua
$code = $code -f $PROJECT_PATH
# Windows Powershell 5 不支持直接指定写入文件的编码为不带 BOM 的 UTF-8（LGHUB 导入带 UTF-8 BOM 的文件存在问题）
$UTF8_NO_BOM = New-Object System.Text.UTF8Encoding $False
[System.IO.File]::WriteAllLines($file.FullName, $code, $UTF8_NO_BOM)