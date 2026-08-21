param([Parameter(Mandatory=$true)][string]$File)
$json = Get-Content -Raw -Encoding UTF8 $File
$bytes = [System.Text.Encoding]::UTF8.GetBytes($json + "`0")
try {
  $r = Invoke-RestMethod -Uri "http://127.0.0.1:8080/api/v1/commands" -Method Post -Body $bytes -ContentType "application/json" -TimeoutSec 120
  $r | ConvertTo-Json -Depth 8 -Compress
} catch { Write-Output "FAIL: $($_.Exception.Message)" }
