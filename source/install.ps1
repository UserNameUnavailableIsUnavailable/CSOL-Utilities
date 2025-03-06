# 注意：对于 Powershell 5.1，此文件必须保存为 UTF-8 BOM，否则字符串将默认以 ANSI 进行编码
# 获取路径，反斜线替换为正斜线
$PROJECT_PATH = (Get-Item $PSScriptRoot\Executor\).FullName -replace("\\", "/")
if (-not $PROJECT_PATH.EndsWith("/"))
{
    $PROJECT_PATH += "/"
}
$code = @"
PATH = "{0}"
---加载 LUA 源文件。
---@param file_name string 文件名（相对路径）。
function Include(file_name)
    if (type(file_name) == "string")
    then
        dofile(PATH .. file_name)
    end
end
Include("Main.lua")
Main()
"@
$file = New-Item -Type File -Path "$PSScriptRoot\Executor.lua" -Force # 创建 Executor.lua
$code = $code -f $PROJECT_PATH # 写入代码
# Powershell 5 不支持直接指定写入文件的编码为不带 BOM 的 UTF-8（LGHUB 导入带 UTF-8 BOM 的文件存在问题）
$utf8_without_bom = New-Object System.Text.UTF8Encoding $False
[System.IO.File]::WriteAllLines($file.FullName, $code, $utf8_without_bom)