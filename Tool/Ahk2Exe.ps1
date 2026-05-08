param(
    [string]$CompilerPath,
    [string]$InputSource,
    [string]$OutputDirectory,
    [string]$BinaryName
)

$err = 0

$ErrorActionPreference = "Stop"

Push-Location
try
{
    Set-Location $PSScriptRoot
    $src = (Get-Item $InputSource).FullName
    $dir = $Null
    if (-not (Test-Path $OutputDirectory))
    {
        $dir = New-Item -Type Directory $OutputDirectory -Force
    }
    else
    {
        $dir = Get-Item $OutputDirectory
        if (-not $dir.PSIsContainer)
        {
            throw "$dir already exists and is not a directory."
        }
    }
    $out = $dir.FullName
    Start-Process -FilePath "$CompilerPath" -ArgumentList "/in `"$src`" /out `"$out\$BinaryName.exe`"" -Wait
}
catch
{
    $err = 1
    Write-Error $_
}
finally
{
    Pop-Location
}

exit($err)
