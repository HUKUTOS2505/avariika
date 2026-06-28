@echo off
setlocal

set "PROJECT_PATH=C:\unrealEngine\avariika"
set "SCRIPT_PATH=%PROJECT_PATH%\Tools\Start-Avariika.ps1"

cd /d "%PROJECT_PATH%"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%SCRIPT_PATH%"
exit /b %ERRORLEVEL%
