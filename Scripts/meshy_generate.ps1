# Meshy API: preview -> refine(PBR) -> download FBX + base color into RawAssets\<Folder>\
# ASCII-only (PowerShell 5.1 mis-reads non-BOM UTF-8 Cyrillic). Key from .meshy_key.
# Usage: .\Scripts\meshy_generate.ps1 -Folder SM_Breaker -Prompt "..." -Model meshy-5 -Polycount 10000
param(
    [Parameter(Mandatory=$true)][string]$Folder,
    [Parameter(Mandatory=$true)][string]$Prompt,
    [string]$Model = 'meshy-5',
    [int]$Polycount = 10000,
    [switch]$Pbr,
    [switch]$HdTexture,
    [switch]$AllMaps   # also download normal/metallic/roughness (off by default: meshy normal crashes UE)
)
$ErrorActionPreference = 'Stop'
$root = 'C:\unrealEngine\avariika'
$key = (Get-Content "$root\.meshy_key" -Raw).Trim()
$base = 'https://api.meshy.ai/openapi/v2/text-to-3d'
$headers = @{ Authorization = "Bearer $key" }
$dest = Join-Path "$root\RawAssets" $Folder
New-Item -ItemType Directory -Force -Path $dest | Out-Null
$wantPbr = [bool]$Pbr
$wantHd = [bool]$HdTexture

function Wait-Task($id, $label) {
    $deadline = (Get-Date).AddMinutes(8)
    while ((Get-Date) -lt $deadline) {
        Start-Sleep -Seconds 8
        $t = Invoke-RestMethod -Uri "$base/$id" -Headers $headers -Method Get
        Write-Host ("  {0}: {1} {2}%" -f $label, $t.status, $t.progress)
        if ($t.status -eq 'SUCCEEDED') { return $t }
        if ($t.status -eq 'FAILED' -or $t.status -eq 'CANCELED') { throw "$label $($t.status)" }
    }
    throw "$label timeout"
}

# 1. PREVIEW (geometry)
$prevBody = @{ mode='preview'; prompt=$Prompt; ai_model=$Model; should_remesh=$true; target_polycount=$Polycount; topology='triangle'; target_formats=@('fbx') } | ConvertTo-Json
Write-Host "PREVIEW ($Model, $Polycount tris)..."
$prevId = (Invoke-RestMethod -Uri $base -Headers $headers -Method Post -Body $prevBody -ContentType 'application/json').result
Write-Host "  preview id: $prevId"
$prev = Wait-Task $prevId 'preview'

# 2. REFINE (textures / PBR). remove_lighting only valid on meshy-6.
$refMap = @{ mode='refine'; preview_task_id=$prevId; ai_model=$Model; enable_pbr=$wantPbr; hd_texture=$wantHd; target_formats=@('fbx') }
if ($Model -eq 'meshy-6') { $refMap['remove_lighting'] = $true }
$refBody = $refMap | ConvertTo-Json
Write-Host "REFINE (pbr=$wantPbr hd=$wantHd)..."
$refId = (Invoke-RestMethod -Uri $base -Headers $headers -Method Post -Body $refBody -ContentType 'application/json').result
Write-Host "  refine id: $refId"
$ref = Wait-Task $refId 'refine'

# 3. FBX
$fbxUrl = $ref.model_urls.fbx
if (-not $fbxUrl) { throw 'no model_urls.fbx in refine response' }
Invoke-WebRequest -Uri $fbxUrl -OutFile (Join-Path $dest ($Folder + '.fbx'))
Write-Host ("  FBX -> {0}\{1}.fbx" -f $dest, $Folder)

# 4. Textures. Base color always; others only with -AllMaps (meshy normal crashes UE import).
if ($ref.texture_urls -and $ref.texture_urls.Count -gt 0) {
    $tex = $ref.texture_urls[0]
    $maps = @('base_color')
    if ($AllMaps) { $maps += @('metallic','roughness','normal') }
    foreach ($m in $maps) {
        if ($tex.$m) { Invoke-WebRequest -Uri $tex.$m -OutFile (Join-Path $dest ($Folder + '_' + $m + '.png')); Write-Host "  tex $m -> ok" }
    }
}

Write-Host ("DONE. Credits: preview {0} + refine {1} = {2}" -f $prev.consumed_credits, $ref.consumed_credits, ($prev.consumed_credits + $ref.consumed_credits))
Get-ChildItem $dest | Select-Object Name, Length
