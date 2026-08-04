param(
    [Parameter(Mandatory = $true)]
    [string]$OutputPath
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing

$ProjectRoot = Split-Path -Parent $PSScriptRoot
$DiagnosticRoot = Join-Path $ProjectRoot 'Saved\CodexDiagnostics\HeadCustomizationUIRedesign_20260713'

$Inputs = @(
    [ordered]@{
        label = 'T_UI_HeadType_01 source PNG'
        role = 'actual_thumbnail_source'
        path = Join-Path $DiagnosticRoot 'GeneratedThumbnailSources\Head\T_UI_HeadType_01.png'
    },
    [ordered]@{
        label = 'T_UI_HeadType_02 source PNG'
        role = 'actual_thumbnail_source'
        path = Join-Path $DiagnosticRoot 'GeneratedThumbnailSources\Head\T_UI_HeadType_02.png'
    },
    [ordered]@{
        label = 'CAP BEGE Type01 source PNG'
        role = 'actual_thumbnail_source'
        path = Join-Path $DiagnosticRoot 'GeneratedThumbnailSources\Headgear\HeadType01\T_UI_Headgear_01_Type01.png'
    },
    [ordered]@{
        label = 'CAP BLACK Type01 source PNG'
        role = 'actual_thumbnail_source'
        path = Join-Path $DiagnosticRoot 'GeneratedThumbnailSources\Headgear\HeadType01\T_UI_Headgear_02_Type01.png'
    },
    [ordered]@{
        label = 'Current Hair card fallback export'
        role = 'generic_category_icon_not_thumbnail'
        path = Join-Path $DiagnosticRoot 'CurrentFallbackExports\CURRENT_HAIR_GENERIC_ICON.png'
    },
    [ordered]@{
        label = 'Current Beard card fallback export'
        role = 'generic_category_icon_not_thumbnail'
        path = Join-Path $DiagnosticRoot 'CurrentFallbackExports\CURRENT_BEARD_GENERIC_ICON.png'
    },
    [ordered]@{
        label = 'CONTROL_HEAD_TYPE_01 opaque control source PNG'
        role = 'control_thumbnail_source'
        path = Join-Path $DiagnosticRoot 'ControlProofSources\CONTROL_HEAD_TYPE_01.png'
    },
    [ordered]@{
        label = 'CONTROL_HEAD_TYPE_02 opaque control source PNG'
        role = 'control_thumbnail_source'
        path = Join-Path $DiagnosticRoot 'ControlProofSources\CONTROL_HEAD_TYPE_02.png'
    },
    [ordered]@{
        label = 'CONTROL_CAP_BEGE_TYPE01 opaque control source PNG'
        role = 'control_thumbnail_source'
        path = Join-Path $DiagnosticRoot 'ControlProofSources\CONTROL_CAP_BEGE_TYPE01.png'
    },
    [ordered]@{
        label = 'CONTROL_HAIR_MOHAWK_TYPE01 opaque control source PNG'
        role = 'control_thumbnail_source'
        path = Join-Path $DiagnosticRoot 'ControlProofSources\CONTROL_HAIR_MOHAWK_TYPE01.png'
    },
    [ordered]@{
        label = 'CONTROL_BEARD_LONG_TYPE01 opaque control source PNG'
        role = 'control_thumbnail_source'
        path = Join-Path $DiagnosticRoot 'ControlProofSources\CONTROL_BEARD_LONG_TYPE01.png'
    },
    [ordered]@{
        label = 'CONTROL_HEAD_TYPE_01 permanent C++ opaque exporter PNG'
        role = 'cpp_exporter_control'
        path = Join-Path $DiagnosticRoot 'ControlProofSources\CONTROL_HEAD_TYPE_01_CPP_EXPORT.png'
    }
)

function Get-PngPixelAudit {
    param([System.Collections.IDictionary]$InputRecord)

    $ResolvedPath = [System.IO.Path]::GetFullPath([string]$InputRecord.path)
    if (-not (Test-Path -LiteralPath $ResolvedPath)) {
        return [ordered]@{
            label = $InputRecord.label
            role = $InputRecord.role
            source_png = $ResolvedPath
            exists = $false
        }
    }

    $Bitmap = [System.Drawing.Bitmap]::FromFile($ResolvedPath)
    try {
        $Width = $Bitmap.Width
        $Height = $Bitmap.Height
        $Rectangle = New-Object System.Drawing.Rectangle(0, 0, $Width, $Height)
        $BitmapData = $Bitmap.LockBits(
            $Rectangle,
            [System.Drawing.Imaging.ImageLockMode]::ReadOnly,
            [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)

        try {
            $Stride = [Math]::Abs($BitmapData.Stride)
            $Bytes = New-Object byte[] ($Stride * $Height)
            [System.Runtime.InteropServices.Marshal]::Copy(
                $BitmapData.Scan0,
                $Bytes,
                0,
                $Bytes.Length)

            $MinR = $MinG = $MinB = $MinA = 255
            $MaxR = $MaxG = $MaxB = $MaxA = 0
            [long]$AlphaZeroPixels = 0
            [long]$AlphaOpaquePixels = 0
            [long]$AlphaZeroNonBlackRgbPixels = 0
            [long]$ChromaticPixels = 0
            [double]$ChromaSum = 0.0
            [double]$AlphaSum = 0.0
            [double]$AlphaSquareSum = 0.0
            [double]$RgbValueSum = 0.0
            [double]$RgbValueSquareSum = 0.0
            $UniqueRgb = [System.Collections.Generic.HashSet[int]]::new()
            $UniqueVisibleRgb = [System.Collections.Generic.HashSet[int]]::new()
            $UniqueHiddenRgb = [System.Collections.Generic.HashSet[int]]::new()
            $UniqueAlpha = [System.Collections.Generic.HashSet[int]]::new()

            for ($Y = 0; $Y -lt $Height; ++$Y) {
                $RowOffset = $Y * $Stride
                for ($X = 0; $X -lt $Width; ++$X) {
                    $Offset = $RowOffset + ($X * 4)
                    $B = [int]$Bytes[$Offset]
                    $G = [int]$Bytes[$Offset + 1]
                    $R = [int]$Bytes[$Offset + 2]
                    $A = [int]$Bytes[$Offset + 3]

                    if ($R -lt $MinR) { $MinR = $R }
                    if ($G -lt $MinG) { $MinG = $G }
                    if ($B -lt $MinB) { $MinB = $B }
                    if ($A -lt $MinA) { $MinA = $A }
                    if ($R -gt $MaxR) { $MaxR = $R }
                    if ($G -gt $MaxG) { $MaxG = $G }
                    if ($B -gt $MaxB) { $MaxB = $B }
                    if ($A -gt $MaxA) { $MaxA = $A }

                    $RgbKey = ($R -shl 16) -bor ($G -shl 8) -bor $B
                    $null = $UniqueRgb.Add($RgbKey)
                    $null = $UniqueAlpha.Add($A)
                    if ($A -eq 0) {
                        ++$AlphaZeroPixels
                        $null = $UniqueHiddenRgb.Add($RgbKey)
                        if ($RgbKey -ne 0) { ++$AlphaZeroNonBlackRgbPixels }
                    }
                    else {
                        $null = $UniqueVisibleRgb.Add($RgbKey)
                    }
                    if ($A -eq 255) { ++$AlphaOpaquePixels }

                    $PixelMax = [Math]::Max($R, [Math]::Max($G, $B))
                    $PixelMin = [Math]::Min($R, [Math]::Min($G, $B))
                    $Chroma = $PixelMax - $PixelMin
                    $ChromaSum += $Chroma
                    if ($Chroma -ge 12) { ++$ChromaticPixels }

                    $AlphaSum += $A
                    $AlphaSquareSum += $A * $A
                    foreach ($Channel in @($R, $G, $B)) {
                        $RgbValueSum += $Channel
                        $RgbValueSquareSum += $Channel * $Channel
                    }
                }
            }

            [long]$PixelCount = [long]$Width * [long]$Height
            [long]$RgbSampleCount = $PixelCount * 3
            $AlphaMean = $AlphaSum / $PixelCount
            $AlphaVariance = [Math]::Max(
                0.0,
                ($AlphaSquareSum / $PixelCount) - ($AlphaMean * $AlphaMean))
            $RgbMean = $RgbValueSum / $RgbSampleCount
            $RgbVariance = [Math]::Max(
                0.0,
                ($RgbValueSquareSum / $RgbSampleCount) - ($RgbMean * $RgbMean))
            $MeanChroma = $ChromaSum / $PixelCount
            $ChromaticFraction = $ChromaticPixels / [double]$PixelCount
            $AlphaZeroFraction = $AlphaZeroPixels / [double]$PixelCount
            $AlphaOpaqueFraction = $AlphaOpaquePixels / [double]$PixelCount
            $AlmostMonochrome = $MeanChroma -lt 3.0 -and $ChromaticFraction -lt 0.02
            $ImageOnlyInAlpha = $UniqueRgb.Count -le 16 -and $AlphaVariance -gt 25.0

            [ordered]@{
                label = $InputRecord.label
                role = $InputRecord.role
                source_png = $ResolvedPath
                exists = $true
                file_bytes = (Get-Item -LiteralPath $ResolvedPath).Length
                width = $Width
                height = $Height
                pixel_format = [string]$Bitmap.PixelFormat
                rgb_range = [ordered]@{
                    r = @($MinR, $MaxR)
                    g = @($MinG, $MaxG)
                    b = @($MinB, $MaxB)
                }
                alpha_range = @($MinA, $MaxA)
                unique_rgb_colors = $UniqueRgb.Count
                unique_rgb_colors_where_alpha_nonzero = $UniqueVisibleRgb.Count
                unique_rgb_colors_where_alpha_zero = $UniqueHiddenRgb.Count
                unique_alpha_values = $UniqueAlpha.Count
                alpha_zero_pixels = $AlphaZeroPixels
                alpha_zero_fraction = [Math]::Round($AlphaZeroFraction, 6)
                alpha_opaque_pixels = $AlphaOpaquePixels
                alpha_opaque_fraction = [Math]::Round($AlphaOpaqueFraction, 6)
                nonblack_rgb_hidden_under_zero_alpha_pixels = $AlphaZeroNonBlackRgbPixels
                mean_rgb_chroma = [Math]::Round($MeanChroma, 4)
                chromatic_pixel_fraction = [Math]::Round($ChromaticFraction, 6)
                rgb_value_standard_deviation = [Math]::Round([Math]::Sqrt($RgbVariance), 4)
                alpha_standard_deviation = [Math]::Round([Math]::Sqrt($AlphaVariance), 4)
                rgb_almost_monochrome = $AlmostMonochrome
                image_information_only_in_alpha = $ImageOnlyInAlpha
                opaque_512x512_requirement_met = ($Width -eq 512 -and $Height -eq 512 -and $MinA -eq 255)
            }
        }
        finally {
            $Bitmap.UnlockBits($BitmapData)
        }
    }
    finally {
        $Bitmap.Dispose()
    }
}

$AuditRows = @($Inputs | ForEach-Object { Get-PngPixelAudit -InputRecord $_ })
$Document = [ordered]@{
    generated_at = (Get-Date).ToString('o')
    audit_definition = [ordered]@{
        chromatic_pixel = 'max(R,G,B)-min(R,G,B) >= 12'
        rgb_almost_monochrome = 'mean RGB chroma < 3 and chromatic-pixel fraction < 0.02'
        image_information_only_in_alpha = 'unique RGB <= 16 and alpha standard deviation > 5'
        required_control_format = '512x512, RGB color, alpha=255 for every pixel'
    }
    findings = $AuditRows
}

$ResolvedOutputPath = [System.IO.Path]::GetFullPath($OutputPath)
$OutputDirectory = Split-Path -Parent $ResolvedOutputPath
New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null
$Document | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $ResolvedOutputPath -Encoding utf8
$Document | ConvertTo-Json -Depth 8
