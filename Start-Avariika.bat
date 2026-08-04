@echo off
setlocal EnableExtensions
title AVARIIKA CODEX HUB

set "PROJECT=C:\unrealEngine\avariika"
set "UE_EXE=C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe"
set "PRIMARY_HOME=%USERPROFILE%.codex"
set "SECOND_HOME=%USERPROFILE%.codex-second"

set "NO_PROXY=localhost,127.0.0.1,::1"
set "no_proxy=localhost,127.0.0.1,::1"

if not exist "%PROJECT%\avariika.uproject" (
echo ERROR: Project file not found:
echo %PROJECT%\avariika.uproject
pause
exit /b 1
)

:menu
cls
echo ==================================
echo          AVARIIKA CODEX HUB
echo ==================================
echo.
echo 1. Primary account - resume session
echo 2. Primary account - new session
echo 3. Second account  - resume session
echo 4. Second account  - new session
echo.
echo 5. Start Unreal Editor
echo 6. Check Unreal MCP
echo.
echo Q. Exit
echo.

choice /C 123456Q /N /M "Select action"

if errorlevel 7 goto end
if errorlevel 6 goto mcp_check
if errorlevel 5 goto unreal
if errorlevel 4 goto second_new
if errorlevel 3 goto second_resume
if errorlevel 2 goto primary_new
if errorlevel 1 goto primary_resume

:primary_resume
set "CODEX_HOME=%PRIMARY_HOME%"
cd /d "%PROJECT%"
codex resume --last --dangerously-bypass-approvals-and-sandbox
pause
goto menu

:primary_new
set "CODEX_HOME=%PRIMARY_HOME%"
cd /d "%PROJECT%"
codex -C "%PROJECT%" --dangerously-bypass-approvals-and-sandbox
pause
goto menu

:second_resume
set "CODEX_HOME=%SECOND_HOME%"
cd /d "%PROJECT%"
codex resume --last --dangerously-bypass-approvals-and-sandbox
pause
goto menu

:second_new
set "CODEX_HOME=%SECOND_HOME%"
cd /d "%PROJECT%"
codex -C "%PROJECT%" --dangerously-bypass-approvals-and-sandbox
pause
goto menu

:unreal
if not exist "%UE_EXE%" (
echo ERROR: UnrealEditor.exe not found:
echo %UE_EXE%
pause
goto menu
)

start "" "%UE_EXE%" "%PROJECT%\avariika.uproject"
echo Unreal Editor is starting...
timeout /t 2 /nobreak >nul
goto menu

:mcp_check
cls
echo Checking Unreal MCP on port 13579...
echo.
curl.exe --noproxy "*" -i --connect-timeout 2 --max-time 5 http://127.0.0.1:13579/mcp
echo.
pause
goto menu

:end
endlocal
exit /b
