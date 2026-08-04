param(
    [Parameter(Mandatory = $true)]
    [string]$OutputPath
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing

Add-Type @'
using System;
using System.Runtime.InteropServices;

public static class AvWindowCaptureNative
{
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT
    {
        public int Left;
        public int Top;
        public int Right;
        public int Bottom;
    }

    [DllImport("user32.dll")]
    public static extern bool GetWindowRect(IntPtr hWnd, out RECT rect);

    [DllImport("user32.dll")]
    public static extern bool SetForegroundWindow(IntPtr hWnd);

    [DllImport("user32.dll")]
    public static extern bool ShowWindow(IntPtr hWnd, int command);
}
'@

$Process = Get-Process UnrealEditor -ErrorAction Stop |
    Where-Object { $_.MainWindowHandle -ne [IntPtr]::Zero } |
    Select-Object -First 1
if (-not $Process) {
    throw 'No UnrealEditor process with a main window was found.'
}

$null = [AvWindowCaptureNative]::ShowWindow($Process.MainWindowHandle, 9)
$null = [AvWindowCaptureNative]::SetForegroundWindow($Process.MainWindowHandle)
Start-Sleep -Milliseconds 300

$Rectangle = New-Object AvWindowCaptureNative+RECT
if (-not [AvWindowCaptureNative]::GetWindowRect($Process.MainWindowHandle, [ref]$Rectangle)) {
    throw 'GetWindowRect failed for the Unreal Editor window.'
}

$Width = $Rectangle.Right - $Rectangle.Left
$Height = $Rectangle.Bottom - $Rectangle.Top
if ($Width -le 0 -or $Height -le 0) {
    throw "Invalid Unreal Editor window bounds: ${Width}x${Height}."
}

$ResolvedOutput = [System.IO.Path]::GetFullPath($OutputPath)
New-Item -ItemType Directory -Path (Split-Path -Parent $ResolvedOutput) -Force |
    Out-Null

$Bitmap = New-Object System.Drawing.Bitmap(
    $Width,
    $Height,
    [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
try {
    $Graphics = [System.Drawing.Graphics]::FromImage($Bitmap)
    try {
        $Graphics.CopyFromScreen(
            $Rectangle.Left,
            $Rectangle.Top,
            0,
            0,
            (New-Object System.Drawing.Size($Width, $Height)),
            [System.Drawing.CopyPixelOperation]::SourceCopy)
    }
    finally {
        $Graphics.Dispose()
    }
    $Bitmap.Save($ResolvedOutput, [System.Drawing.Imaging.ImageFormat]::Png)
}
finally {
    $Bitmap.Dispose()
}

[ordered]@{
    output = $ResolvedOutput
    width = $Width
    height = $Height
    bytes = (Get-Item -LiteralPath $ResolvedOutput).Length
    process_id = $Process.Id
    window_title = $Process.MainWindowTitle
} | ConvertTo-Json
