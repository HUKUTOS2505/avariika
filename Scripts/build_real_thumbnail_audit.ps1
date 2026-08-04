param(
    [string]$DiagnosticRoot = '.\Saved\CodexDiagnostics\HeadCustomizationUIRedesign_20260713'
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing

$ResolvedRoot = [System.IO.Path]::GetFullPath($DiagnosticRoot)
$ManifestPath = Join-Path $ResolvedRoot 'batch_capture_manifest.json'
$AuditPath = Join-Path $ResolvedRoot 'thumbnail_resolution_audit.json'
$ContactSheetRoot = Join-Path $ResolvedRoot 'ContactSheets'
$Manifest = Get-Content -Raw -LiteralPath $ManifestPath | ConvertFrom-Json
if (-not $Manifest.success -or $Manifest.records.Count -ne 76) {
    throw 'Batch capture manifest is not a successful 76-record manifest.'
}

function Get-RealThumbnailPixelAudit {
    param([pscustomobject]$Record)

    $Path = [System.IO.Path]::GetFullPath([string]$Record.source_png)
    if (-not (Test-Path -LiteralPath $Path)) {
        return [pscustomobject][ordered]@{
            category = $Record.category
            head_type = $Record.head_type
            appearance_path = $Record.appearance_path
            source_png = $Path
            exists = $false
            passed = $false
        }
    }

    $Bitmap = [System.Drawing.Bitmap]::FromFile($Path)
    try {
        $Width = $Bitmap.Width
        $Height = $Bitmap.Height
        $Rectangle = New-Object System.Drawing.Rectangle(0, 0, $Width, $Height)
        $Data = $Bitmap.LockBits(
            $Rectangle,
            [System.Drawing.Imaging.ImageLockMode]::ReadOnly,
            [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
        try {
            $Stride = [Math]::Abs($Data.Stride)
            $Bytes = New-Object byte[] ($Stride * $Height)
            [System.Runtime.InteropServices.Marshal]::Copy(
                $Data.Scan0,
                $Bytes,
                0,
                $Bytes.Length)

            $MinR = $MinG = $MinB = $MinA = 255
            $MaxR = $MaxG = $MaxB = $MaxA = 0
            [long]$AlphaZeroPixels = 0
            [long]$AlphaOpaquePixels = 0
            [long]$ChromaticPixels = 0
            [double]$ChromaSum = 0.0
            $UniqueRgb = [System.Collections.Generic.HashSet[int]]::new()
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
                    if ($A -eq 0) { ++$AlphaZeroPixels }
                    if ($A -eq 255) { ++$AlphaOpaquePixels }
                    $null = $UniqueRgb.Add(($R -shl 16) -bor ($G -shl 8) -bor $B)
                    $null = $UniqueAlpha.Add($A)
                    $PixelMax = [Math]::Max($R, [Math]::Max($G, $B))
                    $PixelMin = [Math]::Min($R, [Math]::Min($G, $B))
                    $Chroma = $PixelMax - $PixelMin
                    $ChromaSum += $Chroma
                    if ($Chroma -ge 12) { ++$ChromaticPixels }
                }
            }

            [long]$PixelCount = [long]$Width * [long]$Height
            $MeanChroma = $ChromaSum / $PixelCount
            $ChromaticFraction = $ChromaticPixels / [double]$PixelCount
            $AlmostMonochrome = $MeanChroma -lt 3.0 -and $ChromaticFraction -lt 0.02
            $Passed = $Width -eq 512 -and
                $Height -eq 512 -and
                $MinA -eq 255 -and
                $MaxA -eq 255 -and
                $AlphaOpaquePixels -eq $PixelCount -and
                $UniqueRgb.Count -ge 1000 -and
                -not $AlmostMonochrome

            return [pscustomobject][ordered]@{
                category = $Record.category
                head_type = $Record.head_type
                display_name = $Record.display_name
                appearance_path = $Record.appearance_path
                asset_path = $Record.asset_path
                source_png = $Path
                exists = $true
                file_bytes = (Get-Item -LiteralPath $Path).Length
                file_sha256 = (Get-FileHash -Algorithm SHA256 -LiteralPath $Path).Hash
                width = $Width
                height = $Height
                rgb_range = [ordered]@{
                    r = @($MinR, $MaxR)
                    g = @($MinG, $MaxG)
                    b = @($MinB, $MaxB)
                }
                alpha_range = @($MinA, $MaxA)
                unique_rgb_colors = $UniqueRgb.Count
                unique_alpha_values = $UniqueAlpha.Count
                alpha_zero_pixels = $AlphaZeroPixels
                alpha_opaque_pixels = $AlphaOpaquePixels
                mean_rgb_chroma = [Math]::Round($MeanChroma, 4)
                chromatic_pixel_fraction = [Math]::Round($ChromaticFraction, 6)
                rgb_almost_monochrome = $AlmostMonochrome
                passed = $Passed
            }
        }
        finally {
            $Bitmap.UnlockBits($Data)
        }
    }
    finally {
        $Bitmap.Dispose()
    }
}

$AuditRows = @($Manifest.records | ForEach-Object {
    Get-RealThumbnailPixelAudit -Record $_
})
$DuplicateGroups = @($AuditRows |
    Where-Object { $_.exists } |
    Group-Object file_sha256 |
    Where-Object { $_.Count -gt 1 } |
    ForEach-Object {
        [ordered]@{
            sha256 = $_.Name
            count = $_.Count
            records = @($_.Group | Select-Object category,head_type,display_name,appearance_path,asset_path)
            expected_none_or_headtype_duplicate = @($_.Group | Where-Object {
                $_.category -notmatch '^None' -and $_.category -ne 'HeadType'
            }).Count -eq 0
        }
    })
$UnexpectedDuplicates = @($DuplicateGroups | Where-Object {
    -not $_.expected_none_or_headtype_duplicate
})
$FailedRows = @($AuditRows | Where-Object { -not $_.passed })

$AuditDocument = [ordered]@{
    success = $FailedRows.Count -eq 0 -and $UnexpectedDuplicates.Count -eq 0
    required_resolution = @(512, 512)
    required_alpha = 255
    audited_total = $AuditRows.Count
    passed_total = @($AuditRows | Where-Object { $_.passed }).Count
    failed_total = $FailedRows.Count
    duplicate_groups = $DuplicateGroups
    unexpected_duplicate_groups = $UnexpectedDuplicates
    findings = $AuditRows
}
$AuditDocument | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath $AuditPath -Encoding utf8

function Save-Jpeg {
    param(
        [System.Drawing.Bitmap]$Bitmap,
        [string]$Path
    )
    $Codec = [System.Drawing.Imaging.ImageCodecInfo]::GetImageEncoders() |
        Where-Object { $_.MimeType -eq 'image/jpeg' } |
        Select-Object -First 1
    $Parameters = New-Object System.Drawing.Imaging.EncoderParameters(1)
    $Parameters.Param[0] = New-Object System.Drawing.Imaging.EncoderParameter(
        [System.Drawing.Imaging.Encoder]::Quality,
        [long]92)
    try {
        $Bitmap.Save($Path, $Codec, $Parameters)
    }
    finally {
        $Parameters.Dispose()
    }
}

function New-ContactSheet {
    param(
        [string]$Name,
        [object[]]$Records,
        [int]$Columns = 5
    )

    $CellWidth = 210
    $CellHeight = 238
    $ImageSize = 200
    $Rows = [Math]::Ceiling($Records.Count / [double]$Columns)
    $Sheet = New-Object System.Drawing.Bitmap(
        ($Columns * $CellWidth),
        ([int]$Rows * $CellHeight),
        [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
    try {
        $Graphics = [System.Drawing.Graphics]::FromImage($Sheet)
        try {
            $Graphics.Clear([System.Drawing.Color]::FromArgb(14, 17, 22))
            $Graphics.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
            $Graphics.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
            $Font = New-Object System.Drawing.Font('Arial', 9, [System.Drawing.FontStyle]::Bold)
            $TextBrush = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::FromArgb(238, 239, 242))
            $BorderPen = New-Object System.Drawing.Pen([System.Drawing.Color]::FromArgb(242, 107, 10), 2)
            $StringFormat = New-Object System.Drawing.StringFormat
            $StringFormat.Alignment = [System.Drawing.StringAlignment]::Center
            $StringFormat.LineAlignment = [System.Drawing.StringAlignment]::Center
            $StringFormat.Trimming = [System.Drawing.StringTrimming]::EllipsisCharacter
            try {
                for ($Index = 0; $Index -lt $Records.Count; ++$Index) {
                    $Record = $Records[$Index]
                    $Column = $Index % $Columns
                    $Row = [Math]::Floor($Index / $Columns)
                    $X = ($Column * $CellWidth) + 5
                    $Y = ($Row * $CellHeight) + 5
                    $Image = [System.Drawing.Image]::FromFile([string]$Record.source_png)
                    try {
                        $Graphics.DrawImage($Image, $X, $Y, $ImageSize, $ImageSize)
                    }
                    finally {
                        $Image.Dispose()
                    }
                    $Graphics.DrawRectangle($BorderPen, $X, $Y, $ImageSize, $ImageSize)
                    $Label = if ([string]$Record.category -match '^None') {
                        'NONE'
                    }
                    elseif ($Record.display_name) {
                        [string]$Record.display_name
                    }
                    else {
                        [string]$Record.category
                    }
                    $TextRectangle = New-Object System.Drawing.RectangleF(
                        $X,
                        ($Y + $ImageSize + 2),
                        $ImageSize,
                        28)
                    $Graphics.DrawString($Label, $Font, $TextBrush, $TextRectangle, $StringFormat)
                }
            }
            finally {
                $Font.Dispose()
                $TextBrush.Dispose()
                $BorderPen.Dispose()
                $StringFormat.Dispose()
            }
        }
        finally {
            $Graphics.Dispose()
        }
        $OutputPath = Join-Path $ContactSheetRoot $Name
        Save-Jpeg -Bitmap $Sheet -Path $OutputPath
        return [ordered]@{
            name = $Name
            path = $OutputPath
            item_count = $Records.Count
            width = $Sheet.Width
            height = $Sheet.Height
            bytes = (Get-Item -LiteralPath $OutputPath).Length
        }
    }
    finally {
        $Sheet.Dispose()
    }
}

New-Item -ItemType Directory -Path $ContactSheetRoot -Force | Out-Null
$SheetDefinitions = @(
    [ordered]@{ name = 'HeadTypes_Real.jpg'; category = 'HeadType'; none = $null; head_type = $null; columns = 2 },
    [ordered]@{ name = 'Headgear_Type01_Real.jpg'; category = 'Headgear'; none = 'NoneHeadgear'; head_type = 'Type01'; columns = 5 },
    [ordered]@{ name = 'Headgear_Type02_Real.jpg'; category = 'Headgear'; none = 'NoneHeadgear'; head_type = 'Type02'; columns = 5 },
    [ordered]@{ name = 'Hair_Type01_Real.jpg'; category = 'Hair'; none = 'NoneHair'; head_type = 'Type01'; columns = 3 },
    [ordered]@{ name = 'Hair_Type02_Real.jpg'; category = 'Hair'; none = 'NoneHair'; head_type = 'Type02'; columns = 3 },
    [ordered]@{ name = 'Beard_Type01_Real.jpg'; category = 'Beard'; none = 'NoneBeard'; head_type = 'Type01'; columns = 4 },
    [ordered]@{ name = 'Beard_Type02_Real.jpg'; category = 'Beard'; none = 'NoneBeard'; head_type = 'Type02'; columns = 4 }
)
$ContactSheets = @()
foreach ($Definition in $SheetDefinitions) {
    $Records = @($Manifest.records | Where-Object {
        ($_.category -eq $Definition.category -or $_.category -eq $Definition.none) -and
        ($null -eq $Definition.head_type -or $_.head_type -eq $Definition.head_type)
    })
    $ContactSheets += New-ContactSheet `
        -Name $Definition.name `
        -Records $Records `
        -Columns $Definition.columns
}
$ContactSheetDocument = [ordered]@{
    success = $ContactSheets.Count -eq 7
    sheets = $ContactSheets
}
$ContactSheetDocument | ConvertTo-Json -Depth 6 |
    Set-Content -LiteralPath (Join-Path $ResolvedRoot 'contact_sheet_manifest.json') -Encoding utf8

[ordered]@{
    audit_path = $AuditPath
    audit_success = $AuditDocument.success
    audited_total = $AuditDocument.audited_total
    failed_total = $AuditDocument.failed_total
    unexpected_duplicate_groups = $UnexpectedDuplicates.Count
    contact_sheet_root = $ContactSheetRoot
    contact_sheets = $ContactSheets
} | ConvertTo-Json -Depth 8
