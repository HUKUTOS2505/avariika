@echo off
setlocal

set "PROJECT_PATH=C:\unrealEngine\avariika"
set "CODEX_HOME=C:\Users\admin\.codex"

cd /d "%PROJECT_PATH%"
echo [Avariika Codex Personal] CODEX_HOME=%CODEX_HOME%
codex login --device-auth
exit /b %ERRORLEVEL%
