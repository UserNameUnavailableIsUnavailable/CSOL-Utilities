param(
    [string]$RootDirectory=$(Get-Item $PSScriptRoot).FullName
)

function ConfigureController(
    [string]$RootDirectory
)
{
    $ControllerDirectory = (Get-Item "$RootDirectory\Controller").FullName
    $ExecutorDirectory = (Get-Item "$RootDirectory\Executor").FullName
    $TCGameSetup = (Get-ItemProperty -Path "HKCU:\Software\TCGame").setup
    $GameRootDirectory = (Get-ItemProperty -Path "HKCU:\Software\TCGame\csol").gamepath
    $ControllerLocaleDirectory = (Get-Item "$ControllerDirectory\locales").FullName
    $ControllerModelDirectory = (Get-Item "$ControllerDirectory\locales").FullName

    $ControllerConfig = [ordered]@{
        "Version" = "1.5.7"
        "Locale.Directory" = $ControllerLocaleDirectory
        "Locale.Name" = "zh-CN"
        "Game.IsLauncherAvailable" = $True
        "Game.LauncherExecutablePath" = [System.IO.Path]::Combine($TCGameSetup, "TCGame.exe")
        "Game.LauncherArguments" = @("cso")
        "Game.RootDirectory" = $GameRootDirectory
        "Game.WindowTitle" = "Counter-Strike Online"
        "Executor.CommandFilePath" = [System.IO.Path]::Combine($ExecutorDirectory, "Directives.lua")
        "Engine.Type" = "Classifier"
        "Engine.ModelConfigPath" = [System.IO.Path]::Combine($ControllerModelDirectory, "Classifier", "ResNet", "CSOL-Utilities-ResNet18-800x600.json")
        "Engine.StartGameRoom" = 900
        "Engine.Login" = 300
        "Engine.LoadMap" = 5 * 60
        "Engine.DefaultIdleAfterRestart" = $True
        "Engine.SuppressQuickFullscreen" = $True
        "Engine.SuppressCSOBanner" = $True
        "HotKey.Bindings" = [ordered]@{
            "Null" = @("ctrl", "shift", "alt", "0")
            "NormalIdle" = @("ctrl", "shift", "alt", "1")
            "ExtendedIdle" = @("ctrl", "shift", "alt", "2")
            "BatchCombineParts" = @("ctrl", "shift", "alt", "3")
            "BatchPurchaseItem" = @("ctrl", "shift", "alt", "4")
            "LocateCursor" = @("ctrl", "shift", "alt", "5")
        }
    }

# Export JSON
    $json = $ControllerConfig | ConvertTo-Json
    $UTF8_NO_BOM = New-Object System.Text.UTF8Encoding $False
    [System.IO.File]::WriteAllText("$RootDirectory\Controller.json", $json, $UTF8_NO_BOM)
}

ConfigureController -RootDirectory $RootDirectory
