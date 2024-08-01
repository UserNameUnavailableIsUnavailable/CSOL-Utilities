# $QuickEditCodeSnippet=@" 
# using System;
# using System.Collections.Generic;
# using System.Linq;
# using System.Text;
# using System.Threading.Tasks;
# using System.Runtime.InteropServices;


# public static class DisableConsoleQuickEdit
# {
#     const uint ENABLE_QUICK_EDIT = 0x0040;
#     const int STD_INPUT_HANDLE = -10;
# [DllImport("kernel32.dll", SetLastError = true)]
#     static extern IntPtr GetStdHandle(int nStdHandle);
# [DllImport("kernel32.dll")]
#     static extern bool GetConsoleMode(IntPtr hConsoleHandle, out uint lpMode);
# [DllImport("kernel32.dll")]
#     static extern bool SetConsoleMode(IntPtr hConsoleHandle, uint dwMode);

#     public static bool SetQuickEdit(bool reset)
#     {

#         IntPtr consoleHandle = GetStdHandle(STD_INPUT_HANDLE);
#         uint consoleMode;
#         if (!GetConsoleMode(consoleHandle, out consoleMode))
#         {
#             return false;
#         }
#         if (reset)
#         {
#             consoleMode &= ~ENABLE_QUICK_EDIT;
#         }
#         else
#         {
#             consoleMode |= ENABLE_QUICK_EDIT;
#         }
#         if (!SetConsoleMode(consoleHandle, consoleMode))
#         {
#             return false;
#         }
#         return true;
#     }
# }
# "@

# $QuickEditMode = Add-Type -TypeDefinition $QuickEditCodeSnippet -Language CSharp

# function Set-QuickEdit() 
# {
# [CmdletBinding()]
# param(
# [Parameter(Mandatory=$False, HelpMessage="This switch will disable Console QuickEdit mode.")]
#     [switch]$DisableQuickEdit=$False
# )
# if([DisableConsoleQuickEdit]::SetQuickEdit($DisableQuickEdit))
#     {
#         Write-Output "[MESSAGE] Console Quick-Edit Mode disabled successfully."
#     }
#     else
#     {
#         Write-Output "[ERROR] Attempt to disable Console Quick-Edit Mode failed."
#     }
# }
$currentPrincipal = New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())
if (-not $currentPrincipal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator))
{
    $location = (Get-Location).ToString()
    Start-Process -FilePath powershell -Verb RunAs -ArgumentList "-NoExit -Command `"& {Set-Location `'$location`'; .\Controller.exe}`""
}
else
{
    # Set-QuickEdit -DisableQuickEdit
    .\Controller.exe
}