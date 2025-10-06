param(
    [String]$URL = "https://huggingface.co/UserNameIsUnavailable/CSOL-Utilities-Controller-Models", # URL of the models repository
    [String]$Path = "$PSScriptRoot/models" # Path to store models
)

if (-not (Test-Path $Path)) {
    git lfs install # make sure Git LFS is installed
    git clone $URL $Path # get models
} else {
    Push-Location
    Set-Location $Path
    git pull # update models
    Pop-Location
}