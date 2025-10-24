# Performance Test Script
Write-Host "================================" -ForegroundColor Cyan
Write-Host "DatLichKham Performance Test" -ForegroundColor Cyan
Write-Host "================================" -ForegroundColor Cyan
Write-Host ""

# Check font files
Write-Host "Checking Vietnamese fonts..." -ForegroundColor Yellow
$fonts = @(
    "C:/Windows/Fonts/segoeui.ttf",
    "C:/Windows/Fonts/arialni.ttf",
    "C:/Windows/Fonts/tahomabd.ttf"
)

foreach ($font in $fonts) {
    if (Test-Path $font) {
        Write-Host "  [OK] $font" -ForegroundColor Green
    } else {
        Write-Host "  [MISSING] $font" -ForegroundColor Red
    }
}

Write-Host ""

# Check build
Write-Host "Checking build..." -ForegroundColor Yellow
if (Test-Path "build/Datchikham.exe") {
    $fileInfo = Get-Item "build/Datchikham.exe"
    Write-Host "  [OK] Executable found" -ForegroundColor Green
    Write-Host "  Size: $($fileInfo.Length / 1MB) MB" -ForegroundColor Gray
    Write-Host "  Modified: $($fileInfo.LastWriteTime)" -ForegroundColor Gray
} else {
    Write-Host "  [ERROR] Executable not found!" -ForegroundColor Red
    Write-Host "  Run: cmake --build build" -ForegroundColor Yellow
    exit 1
}

Write-Host ""

# Offer to run
Write-Host "Ready to launch application?" -ForegroundColor Yellow
Write-Host "Press any key to start, or Ctrl+C to cancel..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

Write-Host ""
Write-Host "Starting application..." -ForegroundColor Green
Write-Host "Watch for font loading message in console." -ForegroundColor Gray
Write-Host ""

# Run the app
.\build\Datchikham.exe

Write-Host ""
Write-Host "Application closed." -ForegroundColor Cyan
