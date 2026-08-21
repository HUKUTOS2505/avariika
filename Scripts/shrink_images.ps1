# Downscale large screenshots to small JPGs so Claude can read them without breaking the connection.
# Usage: powershell -File Scripts/shrink_images.ps1 -Src "C:\Users\admin\Videos\Captures" -Filter "RESIDENT*.png" -Out "C:\unrealEngine\avariika\Saved\re7_small" -MaxW 1100 -Quality 70
param(
    [string]$Src = "C:\Users\admin\Videos\Captures",
    [string]$Filter = "RESIDENT*.png",
    [string]$Out = "C:\unrealEngine\avariika\Saved\re7_small",
    [int]$MaxW = 1100,
    [int]$Quality = 70
)
Add-Type -AssemblyName System.Drawing
if (-not (Test-Path $Out)) { New-Item -ItemType Directory -Path $Out -Force | Out-Null }
$codec = [System.Drawing.Imaging.ImageCodecInfo]::GetImageEncoders() | Where-Object { $_.MimeType -eq 'image/jpeg' }
$ep = New-Object System.Drawing.Imaging.EncoderParameters(1)
$ep.Param[0] = New-Object System.Drawing.Imaging.EncoderParameter([System.Drawing.Imaging.Encoder]::Quality, [long]$Quality)
$files = Get-ChildItem -Path $Src -Filter $Filter -File | Sort-Object LastWriteTime
foreach ($f in $files) {
    try {
        $img = [System.Drawing.Image]::FromFile($f.FullName)
        $scale = [Math]::Min(1.0, $MaxW / $img.Width)
        $w = [int]($img.Width * $scale); $h = [int]($img.Height * $scale)
        $bmp = New-Object System.Drawing.Bitmap($w, $h)
        $g = [System.Drawing.Graphics]::FromImage($bmp)
        $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
        $g.DrawImage($img, 0, 0, $w, $h)
        $outName = ($f.BaseName -replace '[^\w\-]', '_') + ".jpg"
        $outPath = Join-Path $Out $outName
        $bmp.Save($outPath, $codec, $ep)
        $g.Dispose(); $bmp.Dispose(); $img.Dispose()
        $kb = [math]::Round((Get-Item $outPath).Length / 1KB)
        Write-Output ("{0}  ->  {1}  ({2} KB, {3}x{4})" -f $f.Name, $outName, $kb, $w, $h)
    } catch {
        Write-Output ("FAIL {0}: {1}" -f $f.Name, $_.Exception.Message)
    }
}
