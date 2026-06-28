$ErrorActionPreference = "Stop"

$ProjectPath = "C:\unrealEngine\avariika"
$CodexHome = "C:\Users\admin\.codex"
$McpHost = "127.0.0.1"
$McpPort = 13579

function Write-Step {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Message,

        [ConsoleColor]$Color = [ConsoleColor]::Gray
    )

    Write-Host ("[Avariika Codex Personal] " + $Message) -ForegroundColor $Color
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

Write-Step "Project path: $ProjectPath" Cyan

if (-not (Test-Path -LiteralPath $ProjectPath)) {
    Write-Step "Project path not found." Red
    exit 1
}

Set-Location -LiteralPath $ProjectPath

if (-not (Get-Command codex -ErrorAction SilentlyContinue)) {
    Write-Step "codex not found in PATH." Red
    exit 1
}

if (-not (Test-Path -LiteralPath $CodexHome)) {
    New-Item -ItemType Directory -Path $CodexHome -Force | Out-Null
}

$env:CODEX_HOME = $CodexHome
Write-Step "CODEX_HOME: $env:CODEX_HOME" Cyan

if (Test-McpTcpPort) {
    Write-Step ("Unreal MCP TCP port open: {0}:{1}" -f $McpHost, $McpPort) Green
}
else {
    Write-Step ("Unreal MCP TCP port closed: {0}:{1}" -f $McpHost, $McpPort) Yellow
}

Write-Step "Starting Codex Personal" Cyan
& codex -C $ProjectPath --dangerously-bypass-approvals-and-sandbox
exit $LASTEXITCODE
