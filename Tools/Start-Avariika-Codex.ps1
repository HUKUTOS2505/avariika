$ErrorActionPreference = "Stop"

$ProjectPath = "C:\unrealEngine\avariika"
$McpHost = "127.0.0.1"
$McpPort = 13579

function Write-Step {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Message,

        [ConsoleColor]$Color = [ConsoleColor]::Gray
    )

    Write-Host ("[Avariika] " + $Message) -ForegroundColor $Color
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

$codexCommand = Get-Command codex -ErrorAction SilentlyContinue
if (-not $codexCommand) {
    Write-Step "codex not found in PATH." Red
    exit 1
}

Write-Step "codex found: $($codexCommand.Source)" Green

if (Test-McpTcpPort) {
    Write-Step "Unreal MCP TCP port open: ${McpHost}:${McpPort}" Green
}
else {
    Write-Step "Unreal MCP TCP port closed: ${McpHost}:${McpPort}" Yellow
}

Write-Step "Starting Codex: resume last session" Cyan
& codex resume --last -C $ProjectPath -s danger-full-access -a never
$resumeExitCode = $LASTEXITCODE

if ($resumeExitCode -ne 0) {
    Write-Step "Codex resume failed with exit code $resumeExitCode. Starting new session." Yellow
    & codex -C $ProjectPath -s danger-full-access -a never
    exit $LASTEXITCODE
}

Write-Step "Codex resume session finished." Green
exit $resumeExitCode
