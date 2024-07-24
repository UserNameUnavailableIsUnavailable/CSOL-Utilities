$currentPrincipal = New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())
if (-not $currentPrincipal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator))
{
    $location = (Get-Location).ToString()
    Start-Process -FilePath powershell -Verb RunAs -WorkingDirectory $location -ArgumentList "-NoExit -Command `"& {Set-Location $location; .\Controller.exe}`""
}
else
{
    .\Controller.exe
}