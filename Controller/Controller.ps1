# 对于陆服用户，控制器运行时会自动检测游戏的根目录和启动命令，因此无需额外设置
# 对于其他服用户，请自行设置游戏根目录和启动命令
# 如果您有意向与我合作并为其他区服提供更完善的支持，请通过邮箱 admin@macrohard.fun 或 GitHub Issues 与我联系
$Options = @{
# "--game-root-dir" = "" # 游戏根目录（即 Bin、Data 所在目录）
# "--launch-game-cmd" = "" # 启动游戏的命令，用于实现免登录启动游戏，需要游戏启动器支持（如 TCGame 可通过 "C:\Program Files (x86)\TCGAME\TCGame.exe" cso 直接免登录启动游戏）
#	"--game-window-title" = "Counter-Strike Online" # 游戏窗口标题，留空则默认为 "Counter-Strike Online"，Steam 服应为 "Counter-Strike Nexon"
	"--locale-resources-dir" = "$PSScriptRoot\Controller\Locales"
	"--language" = "zh-CN"
	"--executor-command-file-path" = "$PSScriptRoot\Executor\Temporary.lua"
	"--idle-engine-type" = "Classifier"
	"--classifier-model-json-path" = "$PSScriptRoot\Controller\Models\Classifier\ResNet\CSOL-Utilities-ResNet18-800x600.json"
}

$Flags = @(
	"--suppress-CSOBanner",
	"--default-idle-after-reconnection"
)

$Parameters = @()

foreach ($key in $Options.Keys)
{
	$val = $Options[$key]
	if ($Null -ne $val -and $val.Length -gt 0)
	{
		$Parameters += "$key"
		$Parameters += "$val"
	}
}

foreach ($flag in $Flags)
{
	$Parameters += $flag
}

$currentPrincipal = New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())
if (-not $currentPrincipal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator))
{
    Start-Process -FilePath powershell -Verb RunAs -ArgumentList "-NoExit -Command `"& { Set-Location `'$PSScriptRoot`'; .\Controller.ps1 }`""
}
else
{
    & "$PSScriptRoot\Controller\Controller.exe" $Parameters
}
