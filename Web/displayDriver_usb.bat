@ECHO OFF
rem if not DEFINED IS_MINIMIZED set IS_MINIMIZED=1 && start "" /min "%~dpnx0" %* && exit
SET ThisScriptsDirectory=%~dp0
SET PowerShellScriptDriver=%ThisScriptsDirectory%displayDriver_usb.ps1
PowerShell -NoProfile -ExecutionPolicy Bypass -Command "& '%PowerShellScriptDriver%'";