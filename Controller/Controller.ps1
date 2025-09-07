$Options = @{
	"--locale-resources-directory" = "$PSScriptRoot\Controller\Locales"
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
