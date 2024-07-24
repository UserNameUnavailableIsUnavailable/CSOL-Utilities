$PROJECT_PATH = (Get-Location).ToString() -replace("\\", "/")
# Powershell 5 不支持 utf8nobom
$file = Get-Item "lua/main.lua"
$content = (Get-Content -Encoding utf8 -Path $file.FullName) -replace("PATH\s*=\s*`".*`"", "PATH = `"$PROJECT_PATH/lua/`"")
$utf8_no_bom = New-Object System.Text.UTF8Encoding $False
[System.IO.File]::WriteAllLines($file.FullName, $content, $utf8_no_bom)
Set-Location $PROJECT_PATH
