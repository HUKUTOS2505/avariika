# Single round-trip to the Claudius UE plugin. HTTP first (port 8080), file mode as fallback.
# Usage: & C:\unrealEngine\avariika\.claude\scripts\claudius.ps1 -Category level -Command spawn_actor -ParametersJson '{"name":"Foo"}'
param(
    [Parameter(Mandatory = $true)][string]$Category,
    [Parameter(Mandatory = $true)][string]$Command,
    [string]$ParametersJson = '{}',
    [int]$TimeoutSec = 30
)

$ErrorActionPreference = 'Stop'
$id = 'cc_' + [guid]::NewGuid().ToString('N').Substring(0, 8)

$request = [ordered]@{
    id         = $id
    category   = $Category
    command    = $Command
    parameters = ($ParametersJson | ConvertFrom-Json)
}
$json = $request | ConvertTo-Json -Depth 10

# --- HTTP mode (fast path). CLAUDIUS.md documents port 8765, but the plugin's real default is 8080. ---
# Trailing NUL works around a plugin bug: ClaudiusSubsystem.cpp:428 reads the body as a C string
# without a terminator, so heap garbage gets appended to the JSON and parsing fails randomly.
try {
    $resp = Invoke-RestMethod -Uri 'http://127.0.0.1:8080/api/v1/commands' -Method Post -Body ($json + [char]0) -ContentType 'application/json' -TimeoutSec $TimeoutSec
    $resp | ConvertTo-Json -Depth 10
    if ($resp.success) { exit 0 } else { exit 1 }
} catch [System.Net.WebException] {
    Write-Warning "HTTP mode unavailable ($($_.Exception.Message)), falling back to file mode."
}

# --- File mode (fallback) ---
$dir = 'C:\unrealEngine\avariika\Plugins\Claudius'
# UTF-8 without BOM (PS 5.1 Out-File would write a BOM)
[System.IO.File]::WriteAllText("$dir\claudius_request.json", $json, (New-Object System.Text.UTF8Encoding $false))

$respPath = "$dir\claudius_response.json"
$deadline = (Get-Date).AddSeconds($TimeoutSec)
while ((Get-Date) -lt $deadline) {
    Start-Sleep -Milliseconds 300
    if (Test-Path $respPath) {
        try { $resp = Get-Content $respPath -Raw | ConvertFrom-Json } catch { continue }
        if ($resp.command_id -eq $id) {
            $resp | ConvertTo-Json -Depth 10
            if ($resp.success) { exit 0 } else { exit 1 }
        }
    }
}
Write-Error "Timed out after ${TimeoutSec}s waiting for Claudius response to $Category.$Command (id=$id). Is the Unreal Editor running with the Claudius plugin?"
exit 1
