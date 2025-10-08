param(
    [String]$URL = "https://huggingface.co/UserNameIsUnavailable/CSOL-Utilities-Controller-Models", # URL of the models repository
    [String]$InstallDir  # dir to store models, optional
)

$DownloadDir = "$PSScriptRoot/models" # Temporary download directory

if (-not (Test-Path $DownloadDir)) {
    git lfs install # make sure Git LFS is installed
    git clone $URL $DownloadDir # get models
} else {
    Push-Location
    Set-Location $DownloadDir
    git pull # update models
    Pop-Location
}

if ($InstallDir) {
    if (Test-Path $InstallDir) {
        Remove-Item -Path $InstallDir -Recurse -Force -ErrorAction Stop
        New-Item -ItemType Directory -Path $InstallDir -ErrorAction Stop | Out-Null
    }
    Get-ChildItem -Path $DownloadDir -Force | Where-Object {
        -not $_.Name.StartsWith(".git") # exclude .git stuff
    } | ForEach-Object {
        Write-Host "Copying $($_.FullName) to $InstallDir" -ForegroundColor Green
        Copy-Item -Path $_.FullName -Destination $InstallDir -Recurse -Force
    }
}