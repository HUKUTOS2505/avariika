$ErrorActionPreference = "Stop"

$ProjectPath = "C:\unrealEngine\avariika"
$CodexHome = "C:\Users\admin\.codex-second"

if (-not (Test-Path -LiteralPath $ProjectPath)) {
    Write-Host "[Avariika Codex Second] Project path not found: $ProjectPath" -ForegroundColor Red
    exit 1
}

Set-Location -LiteralPath $ProjectPath

if (-not (Get-Command codex -ErrorAction SilentlyContinue)) {
    Write-Host "[Avariika Codex Second] codex not found in PATH." -ForegroundColor Red
    exit 1
}

if (-not (Test-Path -LiteralPath $CodexHome)) {
    New-Item -ItemType Directory -Path $CodexHome -Force | Out-Null
}

$env:CODEX_HOME = $CodexHome
Write-Host "[Avariika Codex Second] CODEX_HOME=$env:CODEX_HOME" -ForegroundColor Cyan

& codex -C $ProjectPath --dangerously-bypass-approvals-and-sandbox
exit $LASTEXITCODE
