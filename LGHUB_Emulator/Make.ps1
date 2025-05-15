param (
    [Parameter(Mandatory = $true)]
    [string]$OutputPath
)
$ROOT = $PSScriptRoot
if (-not (Test-Path -Path $OutputPath))
{
    New-Item -Type Directory -Path $OutputPath
}
$SourceFile = "$ROOT\LGHUB_Emulator.c"
$flags = (pkg-config --cflags --libs "lua5.4").split(" ")
$OutputFile = Join-Path -Path $OutputPath -ChildPath "LGHUB_Emulator.dll"
clang -shared -o "$OutputFile" "$SourceFile" $flags