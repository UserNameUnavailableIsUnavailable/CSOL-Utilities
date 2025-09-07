
param(
    [Parameter(Mandatory=$true)]
    [ValidateSet("Release", "Debug", "RelWithDebInfo", "MinSizeRel")]$BuildType, # 构建类型
    [Parameter(Mandatory=$true)]
    [String]$LocalesPath, # 本地化资源所在目录
    [String]$ModelsPath # 模型所在目录（可选）
)

function ReplaceDirectory {
    param (
        [string]$Source,
        [string]$Destination
    )

    if (Test-Path $Destination) {
        Remove-Item -Recurse -Force $Destination
    }

    New-Item -Type Directory -Force -Path $Destination | Out-Null

    $items = Get-ChildItem -Path $Source | Where-Object {
        $_.Name -notmatch '(\.git|\.gitattributes|\.gitignore)$'
    }

    $items | ForEach-Object {
        Copy-Item -Path $_.FullName -Destination $Destination -Recurse -Force
    }
}

# 部署模型到构建目录
if ($Null -ne $ModelsPath -and $ModelsPath.Length -gt 0) {
    ReplaceDirectory -Source $ModelsPath -Destination "$PSScriptRoot/build/$BuildType/Models"
} else {
    Write-Warning "-ModelsPath is not provided, skipping model deployment. Note that you have to manually deploy models for the application to work properly."
}

# 部署本地化资源
ReplaceDirectory -Source $LocalesPath -Destination "$PSScriptRoot/build/$BuildType/Locales"

Copy-Item -Path $PSScriptRoot\Controller.ps1 -Destination "$PSScriptRoot/build/Controller.ps1" -Force