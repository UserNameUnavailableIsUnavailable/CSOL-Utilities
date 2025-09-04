$Options = @{
	"--locale-resources-directory" = "$PSScriptRoot\Controller\Locales"
	"--language" = "zh-CN"
	"--executor-command-file-path" = "$PSScriptRoot\Executor\Temporary.lua"
	"--detect-mode" = "OCR"
	"--OCR-detection-model-path" = "$PSScriptRoot\Controller\Models\OCR\ch_PP-OCRv4_det_infer.onnx"
	"--OCR-recognition-model-path" = "$PSScriptRoot\Controller\Models\OCR\ch_PP-OCRv4_rec_infer.onnx"
	"--OCR-dictionary-path" = "$PSScriptRoot\Controller\Models\OCR\dictionary.txt"
	"--OCR-keywords-path" = "$PSScriptRoot\Controller\Models\OCR\keywords.json"
}

$Flags = @(
	"--suppress-CSOBanner",
	"--default-idle-after-reconnection"
)

$Parameters = @()

foreach ($key in $Options.Keys)
{
	$val = $Options[$key]
	if ($val -ne $Null -and $val.Length -gt 0)
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
