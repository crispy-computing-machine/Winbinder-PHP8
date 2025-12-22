# Wrapper script to run PHP 8.3 compatibility verification
# This script is in the root directory for easy access

$scriptPath = Join-Path $PSScriptRoot "docs/php83-compatibility/verify_fixes.ps1"

if (Test-Path $scriptPath) {
    & $scriptPath
} else {
    Write-Host "Error: Verification script not found at $scriptPath" -ForegroundColor Red
    Write-Host "Please ensure docs/php83-compatibility/verify_fixes.ps1 exists" -ForegroundColor Red
}
