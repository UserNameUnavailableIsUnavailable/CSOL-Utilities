
$Config = @{
#	自行指定游戏根目录和游戏启动命令示例：
#	GameRootDirectory = "C:\Users\Silver\Games\Tiancity\csol\";
#	LaunchGameCmd = '"C:\Program Files (x86)\TCGame\tcgame.exe" cso';
	MaxWaitTimeInRoom = 600;
}

$Parameters = @()

foreach ($key in $Config.Keys)
{
	$val = $Config[$key]
	if ($val -ne $Null -and $val.Length -gt 0)
	{
		$Parameters += "--$key"
		$Parameters += "$val"
	}
}

$currentPrincipal = New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())
if (-not $currentPrincipal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator))
{
    Start-Process -FilePath powershell -Verb RunAs -ArgumentList "-NoExit -Command `"& { Set-Location `'$PSScriptRoot`'; .\Controller.ps1 }`""
}
else
{
    .\Controller.exe $Parameters
}
