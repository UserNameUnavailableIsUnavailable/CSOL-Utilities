$Config = @{
	# 注意每项配置以分号结尾
	# 示例：GameRootDirectory = 'C:\Users\Silver\Games\Tiancity\csol\';
	# 示例：LaunchGameCmd = '"C:\Program Files (x86)\TCGame\TCGame.exe" cso';
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
