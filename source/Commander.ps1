
$CSO_InstallDir =  Get-ItemPropertyValue registry::hkey_current_user\software\tcgame\csol -Name gamepath
if (-not ($CSO_InstallDir -match(".*\\$"))) # 判断是否以 \ 作为结尾
{
    $CSO_InstallDir += "\";
}
$CSO_ErrorLogDir = $CSO_InstallDir + "bin\Error.log";
$Content = Get-Content $CSO_ErrorLogDir;


while ($True)
{
    for ($i = $Content.count - 1; $i -ge 0; $i--)
    {
        if ($Content[$i] -match "[0-9]{2}:[0-9]{2}:[0-9]{2}\.[0-9]{3}.*Join HostServer")
        {
            Out-File -FilePath ./COMMAND.LUA -Encoding utf8NoBOM -InputObject 'COMMAND = "[ROUND] START_PLAY"'
            Out-File -FilePath ./COMMAND.LUA -Append -Encoding utf8NoBOM -InputObject 'PLAYER = 0'
        }
        elseif ($Content[$i] -match "[0-9]{2}:[0-9]{2}:[0-9]{2}\.[0-9]{3}.*Result Confirm")
        {
            Out-File -FilePath ./COMMAND.LUA -Encoding utf8NoBOM -InputObject 'COMMAND = "[ROUND] START_GAME"'
        }
    }
    Start-Sleep -MilliSeconds 5000
}