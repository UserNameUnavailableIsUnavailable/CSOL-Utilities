$PROJECT_PATH = (Get-Location).ToString() -replace("\\", "/")

$file = Get-Item "lua/main.lua"
(Get-Content -Encoding utf8 -Path $file.Fullname) -replace("PATH\s*=\s*`".*`"", "PATH = `"$PROJECT_PATH/lua/`"") | Out-File -FilePath $file.FullName -Encoding utf8

Set-Location $PROJECT_PATH
