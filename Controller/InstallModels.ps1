param(
    [String]$Url = "https://huggingface.co/UserNameIsUnavailable/CSOL-Utilities-Controller-Models",
    [ValidateSet("Debug", "Release")]$Target = "Release"
)
$ErrorActionPreference = "Stop"
$Path = "$PSScriptRoot/build/downloads/models"
$Clone = $True
if (Test-Path $Path) {
    Push-Location
    Set-Location $Path
    git status
    if (-Not $?) {
        Pop-Location
        Write-Error "Broken git repository." -ErrorAction Continue
        Remove-Item -Recurse -Force $Path
    } else {
        $Clone = $False
        git checkout main
        git pull
        Pop-Location
    }
}
if ($Clone) {
    git clone $Url $Path
}
New-Item -ItemType Directory -Force -Path "$PSScriptRoot/build/$Target/models" | Out-Null
Copy-Item -Recurse -Force -Path "$Path/*" -Exclude ".git" -Destination "$PSScriptRoot/build/$Target/models"
