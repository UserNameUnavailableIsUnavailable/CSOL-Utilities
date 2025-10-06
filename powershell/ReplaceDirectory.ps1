function Replace-Directory {
    param(
        [Parameter(Mandatory=$True)]
        [String]$Source,
        [Parameter(Mandatory=$True)]
        [String]$Destination
    )
    if (-not (Test-Path -Path $Source)) {
        Write-Warning "Source path '$Source' does not exist, Skipping."
        return
    }
    if (-not Test-Path -Path $Destination) {
        New-Item -Type Directory -Path $Destination | Out-Null
    } else {
        Get-ChildItem -Path $Destination | Remove-Item -Recurse -Force
    }
    Get-ChildItem -Path $Source | Where-Object {
        $_.Name -notmatch '(\.git|\.gitattributes|\.gitignore)$'
    } | ForEach-Object {
        Copy-Item -Path $_.FullName -Destination $Destination -Recurse -Force
    }
}