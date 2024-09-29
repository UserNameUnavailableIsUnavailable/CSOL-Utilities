$PROJECT_PATH = (Get-Location).ToString() -replace("\\", "/")
# Powershell 5 不支持 utf8nobom
$file = Get-Item "Main.lua"
$content = (Get-Content -Encoding utf8 -Path $file.FullName) -replace("PATH\s*=\s*`".*`"", "PATH = `"$PROJECT_PATH/Executor/`"")
$utf8_no_bom = New-Object System.Text.UTF8Encoding $False
[System.IO.File]::WriteAllLines($file.FullName, $content, $utf8_no_bom)