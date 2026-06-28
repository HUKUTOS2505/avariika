$ErrorActionPreference = "Stop"
[Console]::OutputEncoding = [System.Text.UTF8Encoding]::new()
[Console]::InputEncoding = [System.Text.UTF8Encoding]::new()
$OutputEncoding = [System.Text.UTF8Encoding]::new()

$ProjectPath = "C:\unrealEngine\avariika"
$ProjectFile = Join-Path $ProjectPath "avariika.uproject"
$UnrealEditor = "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe"
$BuildScript = "C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat"
$McpHost = "127.0.0.1"
$McpPort = 13579
$BuildLogDir = Join-Path $ProjectPath "Saved\Logs"

$Profiles = [ordered]@{
    Personal = @{
        Name = "Personal"
        Label = "Personal"
        CodexHome = "C:\Users\admin\.codex"
    }
    Second = @{
        Name = "Second"
        Label = "Second"
        CodexHome = "C:\Users\admin\.codex-second"
    }
}

function Write-Hub {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Message,

        [ConsoleColor]$Color = [ConsoleColor]::Gray,

        [switch]$NoNewLine
    )

    if ($NoNewLine) {
        Write-Host $Message -ForegroundColor $Color -NoNewline
    }
    else {
        Write-Host $Message -ForegroundColor $Color
    }
}

function Write-Ok { param([string]$Message) Write-Hub "✓ $Message" Green }
function Write-Fail { param([string]$Message) Write-Hub "✗ $Message" Red }
function Write-Warn { param([string]$Message) Write-Hub "⚠ $Message" Yellow }
function Write-Info { param([string]$Message) Write-Hub $Message Cyan }

function Invoke-HubOperation {
    param(
        [Parameter(Mandatory = $true)]
        [scriptblock]$Action
    )

    try {
        & $Action
    }
    catch {
        Write-Fail "Операция завершилась ошибкой: $($_.Exception.Message)"
    }
}

function Pause-Hub {
    Write-Host ""
    Write-Hub "Нажмите Enter, чтобы вернуться в меню..." DarkGray
    [void][Console]::ReadLine()
}

function Ensure-Project {
    if (-not (Test-Path -LiteralPath $ProjectPath)) {
        Write-Fail "Путь проекта не найден: $ProjectPath"
        return $false
    }

    if (-not (Test-Path -LiteralPath $ProjectFile)) {
        Write-Fail "Файл проекта не найден: $ProjectFile"
        return $false
    }

    Set-Location -LiteralPath $ProjectPath
    return $true
}

function Get-CodexCommand {
    $command = Get-Command codex -ErrorAction SilentlyContinue
    if (-not $command) {
        Write-Fail "Codex не найден в PATH."
        return $null
    }

    return $command
}

function Ensure-CodexProfile {
    param(
        [Parameter(Mandatory = $true)]
        [ValidateSet("Personal", "Second")]
        [string]$ProfileName
    )

    $profile = $Profiles[$ProfileName]
    if (-not (Test-Path -LiteralPath $profile.CodexHome)) {
        New-Item -ItemType Directory -Path $profile.CodexHome -Force | Out-Null
    }

    $env:CODEX_HOME = $profile.CodexHome
    Write-Ok "Выбран профиль Codex: $($profile.Label)"
    Write-Info "CODEX_HOME: $env:CODEX_HOME"
    return $profile
}

function Get-CurrentCodexProfileName {
    foreach ($name in $Profiles.Keys) {
        if ($env:CODEX_HOME -and ($env:CODEX_HOME.TrimEnd("\") -ieq $Profiles[$name].CodexHome.TrimEnd("\"))) {
            return $Profiles[$name].Label
        }
    }

    if ($env:CODEX_HOME) {
        return "Внешний ($env:CODEX_HOME)"
    }

    return "не выбран"
}

function Get-CodexAccountSummary {
    param([hashtable]$Profile)

    $authPath = Join-Path $Profile.CodexHome "auth.json"
    if (-not (Test-Path -LiteralPath $authPath)) {
        return "не выполнен вход"
    }

    try {
        $authText = Get-Content -Raw -LiteralPath $authPath -ErrorAction Stop
        $auth = $authText | ConvertFrom-Json -ErrorAction Stop
        foreach ($property in @("email", "account_id", "user_id", "id")) {
            if ($auth.PSObject.Properties.Name -contains $property -and $auth.$property) {
                return [string]$auth.$property
            }
        }
    }
    catch {
        return "auth.json найден"
    }

    return "auth.json найден"
}

function Get-CurrentCodexAccountSummary {
    foreach ($name in $Profiles.Keys) {
        if ($env:CODEX_HOME -and ($env:CODEX_HOME.TrimEnd("\") -ieq $Profiles[$name].CodexHome.TrimEnd("\"))) {
            return Get-CodexAccountSummary -Profile $Profiles[$name]
        }
    }

    return "не выбран"
}

function Get-CodexSessionId {
    $home = $env:CODEX_HOME
    if (-not $home -or -not (Test-Path -LiteralPath $home)) {
        return "нет"
    }

    $sessionRoot = Join-Path $home "sessions"
    if (-not (Test-Path -LiteralPath $sessionRoot)) {
        return "нет"
    }

    $latest = Get-ChildItem -LiteralPath $sessionRoot -Recurse -File -ErrorAction SilentlyContinue |
        Sort-Object LastWriteTime -Descending |
        Select-Object -First 1

    if (-not $latest) {
        return "нет"
    }

    return [System.IO.Path]::GetFileNameWithoutExtension($latest.Name)
}

function Test-UnrealRunning {
    return [bool](Get-Process -Name UnrealEditor -ErrorAction SilentlyContinue)
}

function Get-UnrealProcesses {
    return @(Get-Process -Name UnrealEditor -ErrorAction SilentlyContinue)
}

function Test-McpTcpPort {
    $client = $null
    try {
        $client = [System.Net.Sockets.TcpClient]::new()
        $async = $client.BeginConnect($McpHost, $McpPort, $null, $null)
        if (-not $async.AsyncWaitHandle.WaitOne(1000, $false)) {
            return $false
        }

        $client.EndConnect($async)
        return $true
    }
    catch {
        return $false
    }
    finally {
        if ($client) {
            $client.Close()
            $client.Dispose()
        }
    }
}

function Wait-Mcp {
    param([int]$TimeoutSeconds = 120)

    Write-Info "Waiting for Unreal MCP..."
    $deadline = (Get-Date).AddSeconds($TimeoutSeconds)
    while ((Get-Date) -lt $deadline) {
        if (Test-McpTcpPort) {
            Write-Host ""
            Write-Ok "Connected."
            return $true
        }

        Write-Hub "." DarkGray -NoNewLine
        Start-Sleep -Seconds 2
    }

    Write-Host ""
    Write-Fail ("MCP не ответил за {0} секунд: {1}:{2}" -f $TimeoutSeconds, $McpHost, $McpPort)
    return $false
}

function Start-UnrealEditor {
    if (-not (Ensure-Project)) { return $false }

    if (Test-UnrealRunning) {
        Write-Ok "Unreal Editor уже запущен. Новый экземпляр не запускаю."
        return $true
    }

    if (-not (Test-Path -LiteralPath $UnrealEditor)) {
        Write-Fail "Unreal Editor не найден: $UnrealEditor"
        return $false
    }

    Write-Info "Запускаю Unreal Editor..."
    Start-Process -FilePath $UnrealEditor -ArgumentList "`"$ProjectFile`"" -WorkingDirectory $ProjectPath | Out-Null
    Write-Ok "Команда запуска отправлена."
    return $true
}

function Stop-UnrealEditor {
    $processes = Get-UnrealProcesses
    if ($processes.Count -eq 0) {
        Write-Ok "Unreal Editor не запущен."
        return $true
    }

    Write-Warn "Закрываю Unreal Editor..."
    foreach ($process in $processes) {
        try {
            if ($process.MainWindowHandle -ne 0) {
                [void]$process.CloseMainWindow()
            }
            else {
                Stop-Process -Id $process.Id -Force -ErrorAction Stop
            }
        }
        catch {
            Write-Warn "Не удалось закрыть процесс UnrealEditor PID $($process.Id): $($_.Exception.Message)"
        }
    }

    $deadline = (Get-Date).AddSeconds(30)
    while ((Get-Date) -lt $deadline) {
        if (-not (Test-UnrealRunning)) {
            Write-Ok "Unreal Editor закрыт."
            return $true
        }
        Start-Sleep -Seconds 1
    }

    Write-Warn "Unreal Editor не закрылся за 30 секунд."
    return $false
}

function Restart-UnrealEditor {
    Stop-UnrealEditor | Out-Null
    Start-Sleep -Seconds 2
    Start-UnrealEditor | Out-Null
}

function Test-Mcp {
    if (Test-McpTcpPort) {
        Write-Ok ("MCP доступен: {0}:{1}" -f $McpHost, $McpPort)
        return $true
    }

    Write-Fail ("MCP недоступен: {0}:{1}" -f $McpHost, $McpPort)
    return $false
}

function Restart-Mcp {
    Write-Info "MCP живет внутри Unreal Editor. Перезапускаю Unreal для перезапуска MCP."
    Restart-UnrealEditor
    Wait-Mcp -TimeoutSeconds 180 | Out-Null
}

function Start-CodexProfile {
    param(
        [Parameter(Mandatory = $true)]
        [ValidateSet("Personal", "Second")]
        [string]$ProfileName
    )

    if (-not (Ensure-Project)) { return }
    $codexCommand = Get-CodexCommand
    if (-not $codexCommand) { return }

    Ensure-CodexProfile -ProfileName $ProfileName | Out-Null
    Write-Ok "Codex найден: $($codexCommand.Source)"
    Write-Info "Запускаю Codex в проекте..."
    & codex -C $ProjectPath --dangerously-bypass-approvals-and-sandbox
    Write-Info "Codex завершился с кодом $LASTEXITCODE."
}

function Resume-CodexProfile {
    param(
        [Parameter(Mandatory = $true)]
        [ValidateSet("Personal", "Second")]
        [string]$ProfileName
    )

    if (-not (Ensure-Project)) { return }
    $codexCommand = Get-CodexCommand
    if (-not $codexCommand) { return }

    Ensure-CodexProfile -ProfileName $ProfileName | Out-Null
    Write-Ok "Codex найден: $($codexCommand.Source)"
    Write-Info "Возобновляю последнюю сессию Codex..."
    & codex resume --last -C $ProjectPath --dangerously-bypass-approvals-and-sandbox
    Write-Info "Codex resume завершился с кодом $LASTEXITCODE."
}

function Login-CodexProfile {
    param(
        [Parameter(Mandatory = $true)]
        [ValidateSet("Personal", "Second")]
        [string]$ProfileName
    )

    if (-not (Ensure-Project)) { return }
    $codexCommand = Get-CodexCommand
    if (-not $codexCommand) { return }

    Ensure-CodexProfile -ProfileName $ProfileName | Out-Null
    Write-Ok "Codex найден: $($codexCommand.Source)"
    Write-Info "Запускаю вход Codex через device auth..."
    & codex login --device-auth

    if ($LASTEXITCODE -eq 0) {
        Write-Ok "Вход завершен."
    }
    else {
        Write-Fail "Вход завершился с кодом $LASTEXITCODE."
    }
}

function Get-GitBranch {
    if (-not (Ensure-Project)) { return "недоступно" }
    $branch = git branch --show-current 2>$null
    if ($LASTEXITCODE -eq 0 -and $branch) { return $branch }
    return "недоступно"
}

function Get-GitDirtyCount {
    if (-not (Ensure-Project)) { return 0 }
    $status = @(git status --porcelain 2>$null)
    if ($LASTEXITCODE -ne 0) { return "недоступно" }
    return $status.Count
}

function Get-GitLastCommit {
    if (-not (Ensure-Project)) { return "недоступно" }
    $commit = git log -1 --pretty=format:"%h %s" 2>$null
    if ($LASTEXITCODE -eq 0 -and $commit) { return $commit }
    return "недоступно"
}

function Get-GitCommitHash {
    if (-not (Ensure-Project)) { return "недоступно" }
    $commit = git rev-parse --short HEAD 2>$null
    if ($LASTEXITCODE -eq 0 -and $commit) { return $commit }
    return "недоступно"
}

function Show-GitStatus {
    if (-not (Ensure-Project)) { return }

    Write-Hub "Git" Yellow
    Write-Hub "Ветка: $(Get-GitBranch)" White
    Write-Hub "Измененных файлов: $(Get-GitDirtyCount)" White
    Write-Hub "Последний коммит: $(Get-GitLastCommit)" White
    Write-Host ""

    $status = @(git -c core.quotePath=false status --short 2>$null)
    if ($status.Count -eq 0) {
        Write-Ok "Рабочее дерево чистое."
        return
    }

    Write-Warn "Файлы с изменениями:"
    $status | Select-Object -First 80 | ForEach-Object { Write-Hub "  $_" Gray }
    if ($status.Count -gt 80) {
        Write-Warn "Показаны первые 80 строк из $($status.Count)."
    }
}

function Show-ProjectStatus {
    if (-not (Ensure-Project)) { return }

    $unrealStatus = if (Test-UnrealRunning) { "запущен" } else { "не запущен" }
    $mcpStatus = if (Test-McpTcpPort) { "доступен" } else { "недоступен" }
    $dirtyCount = Get-GitDirtyCount
    $dirtyState = if ($dirtyCount -eq 0) { "чисто" } else { "$dirtyCount файлов" }

    Write-Hub "Статус проекта" Yellow
    Write-Hub "Путь проекта: $ProjectPath" White
    Write-Hub "Git ветка: $(Get-GitBranch)" White
    Write-Hub "Git dirty state: $dirtyState" White
    Write-Hub "Текущий коммит: $(Get-GitCommitHash)" White
    Write-Hub "Unreal: $unrealStatus" $(if (Test-UnrealRunning) { "Green" } else { "Yellow" })
    $mcpColor = if (Test-McpTcpPort) { "Green" } else { "Red" }
    Write-Hub ("MCP: {0} ({1}:{2})" -f $mcpStatus, $McpHost, $McpPort) $mcpColor
    Write-Hub "Текущий Codex профиль: $(Get-CurrentCodexProfileName)" White
    Write-Hub "Текущий аккаунт: $(Get-CurrentCodexAccountSummary)" White
    Write-Hub "Текущая session id: $(Get-CodexSessionId)" White
    Write-Host ""
    Write-Hub "Профили Codex:" Cyan
    foreach ($name in $Profiles.Keys) {
        $profile = $Profiles[$name]
        $exists = Test-Path -LiteralPath $profile.CodexHome
        $account = Get-CodexAccountSummary -Profile $profile
        $folderState = if ($exists) { "папка есть" } else { "папки нет" }
        Write-Hub ("  {0}: {1} | {2} | {3}" -f $profile.Label, $profile.CodexHome, $folderState, $account) Gray
    }
}

function Invoke-Build {
    if (-not (Ensure-Project)) { return }
    if (-not (Test-Path -LiteralPath $BuildScript)) {
        Write-Fail "Build.bat не найден: $BuildScript"
        return
    }

    if (Test-UnrealRunning) {
        Write-Warn "Для полного Build.bat закрываю Unreal Editor."
        Stop-UnrealEditor | Out-Null
    }

    if (-not (Test-Path -LiteralPath $BuildLogDir)) {
        New-Item -ItemType Directory -Path $BuildLogDir -Force | Out-Null
    }

    $logPath = Join-Path $BuildLogDir ("DeveloperHub_Build_{0}.log" -f (Get-Date -Format "yyyyMMdd_HHmmss"))
    $arguments = @(
        "avariikaEditor",
        "Win64",
        "Development",
        "-project=$ProjectFile",
        "-WaitMutex"
    )

    Write-Info "Запускаю build..."
    Write-Hub "`"$BuildScript`" $($arguments -join ' ')" DarkGray
    & $BuildScript @arguments *> $logPath
    $exitCode = $LASTEXITCODE

    if ($exitCode -eq 0) {
        Write-Ok "Build успешно завершен."
    }
    else {
        Write-Fail "Build завершился с ошибкой. Код: $exitCode"
        Write-Hub "Лог: $logPath" Yellow
        $errors = Select-String -LiteralPath $logPath -Pattern ": error| error |ERROR|Fatal" -CaseSensitive:$false -ErrorAction SilentlyContinue |
            Select-Object -First 20
        if ($errors) {
            Write-Hub "Первые найденные ошибки:" Yellow
            $errors | ForEach-Object { Write-Hub "  $($_.Line)" Red }
        }
    }
}

function Get-LatestUnrealLog {
    $logRoot = Join-Path $ProjectPath "Saved\Logs"
    if (-not (Test-Path -LiteralPath $logRoot)) {
        return $null
    }

    return Get-ChildItem -LiteralPath $logRoot -File -Filter "*.log" -ErrorAction SilentlyContinue |
        Sort-Object LastWriteTime -Descending |
        Select-Object -First 1
}

function Invoke-SmokeTest {
    if (-not (Ensure-Project)) { return }

    Write-Info "Smoke Test: запуск редактора, ожидание MCP, проверка логов."
    if (-not (Start-UnrealEditor)) { return }

    $mcpOk = Wait-Mcp -TimeoutSeconds 180
    $latestLog = Get-LatestUnrealLog

    Write-Host ""
    Write-Hub "Smoke Test Summary" Yellow
    $unrealSmokeRunning = Test-UnrealRunning
    Write-Hub ("Unreal: {0}" -f $(if ($unrealSmokeRunning) { "запущен" } else { "не запущен" })) $(if ($unrealSmokeRunning) { "Green" } else { "Red" })
    Write-Hub ("MCP: {0}" -f $(if ($mcpOk) { "доступен" } else { "недоступен" })) $(if ($mcpOk) { "Green" } else { "Red" })

    if (-not $latestLog) {
        Write-Warn "Лог Unreal не найден."
        return
    }

    Write-Hub "Лог: $($latestLog.FullName)" White
    $issues = Select-String -LiteralPath $latestLog.FullName -Pattern "Fatal|Ensure|: Error:" -CaseSensitive:$false -ErrorAction SilentlyContinue
    if ($issues) {
        Write-Fail "Найдены проблемы в логе: $($issues.Count)"
        $issues | Select-Object -First 20 | ForEach-Object { Write-Hub "  $($_.Line)" Red }
    }
    else {
        Write-Ok "Fatal, Ensure и ': Error:' в последнем логе не найдены."
    }
}

function Show-Header {
    Clear-Host
    Write-Hub "========================================================" DarkCyan
    Write-Hub "            AVARIIKA DEVELOPER HUB v2" Yellow
    Write-Hub "========================================================" DarkCyan
    Write-Host ""
    Write-Hub "Project  Git  Unreal  MCP  Codex" DarkGray
    Write-Host ""
    Write-Hub "Проект: $ProjectPath" DarkGray
    $headerBranch = Get-GitBranch
    $headerDirty = Get-GitDirtyCount
    $headerUnreal = if (Test-UnrealRunning) { "✓" } else { "✗" }
    $headerMcp = if (Test-McpTcpPort) { "✓" } else { "✗" }
    Write-Hub ("Ветка: {0} | Dirty: {1} | Unreal: {2} | MCP: {3}" -f $headerBranch, $headerDirty, $headerUnreal, $headerMcp) DarkGray
    Write-Host ""
}

function Show-Menu {
    Write-Hub "--------------------------------------------------------" DarkGray
    Write-Hub "[1]  Запустить Personal" Gray
    Write-Hub "[2]  Запустить Second" Gray
    Write-Hub "--------------------------------------------------------" DarkGray
    Write-Hub "[3]  Продолжить Personal" Gray
    Write-Hub "[4]  Продолжить Second" Gray
    Write-Hub "--------------------------------------------------------" DarkGray
    Write-Hub "[5]  Запустить Unreal" Gray
    Write-Hub "[6]  Перезапустить Unreal" Gray
    Write-Hub "--------------------------------------------------------" DarkGray
    Write-Hub "[7]  Проверить MCP" Gray
    Write-Hub "[8]  Перезапустить MCP" Gray
    Write-Hub "--------------------------------------------------------" DarkGray
    Write-Hub "[9]  Статус проекта" Gray
    Write-Hub "[10] Git Status" Gray
    Write-Hub "--------------------------------------------------------" DarkGray
    Write-Hub "[11] Build" Gray
    Write-Hub "[12] Smoke Test" Gray
    Write-Hub "--------------------------------------------------------" DarkGray
    Write-Hub "[13] Войти Personal" Gray
    Write-Hub "[14] Войти Second" Gray
    Write-Hub "--------------------------------------------------------" DarkGray
    Write-Hub "[0]  Выход" Gray
    Write-Hub "--------------------------------------------------------" DarkGray
    Write-Host ""
}

while ($true) {
    Show-Header
    Show-Menu
    $choice = Read-Host "Введите номер действия"
    Write-Host ""

    switch ($choice) {
        "1" { Invoke-HubOperation { Start-CodexProfile -ProfileName Personal }; Pause-Hub }
        "2" { Invoke-HubOperation { Start-CodexProfile -ProfileName Second }; Pause-Hub }
        "3" { Invoke-HubOperation { Resume-CodexProfile -ProfileName Personal }; Pause-Hub }
        "4" { Invoke-HubOperation { Resume-CodexProfile -ProfileName Second }; Pause-Hub }
        "5" { Invoke-HubOperation { Start-UnrealEditor | Out-Null }; Pause-Hub }
        "6" { Invoke-HubOperation { Restart-UnrealEditor }; Pause-Hub }
        "7" { Invoke-HubOperation { Test-Mcp | Out-Null }; Pause-Hub }
        "8" { Invoke-HubOperation { Restart-Mcp }; Pause-Hub }
        "9" { Invoke-HubOperation { Show-ProjectStatus }; Pause-Hub }
        "10" { Invoke-HubOperation { Show-GitStatus }; Pause-Hub }
        "11" { Invoke-HubOperation { Invoke-Build }; Pause-Hub }
        "12" { Invoke-HubOperation { Invoke-SmokeTest }; Pause-Hub }
        "13" { Invoke-HubOperation { Login-CodexProfile -ProfileName Personal }; Pause-Hub }
        "14" { Invoke-HubOperation { Login-CodexProfile -ProfileName Second }; Pause-Hub }
        "0" {
            Write-Info "Выход из AVARIIKA DEVELOPER HUB v2."
            exit 0
        }
        default {
            Write-Warn "Неизвестный пункт меню: $choice"
            Pause-Hub
        }
    }
}

