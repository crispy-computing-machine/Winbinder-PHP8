# PHP 8.3.29 Compatibility Verification Script

Write-Host "=== PHP 8.3.29 Compatibility Check ===" -ForegroundColor Cyan
Write-Host ""

# Get the root directory (two levels up from this script)
$rootDir = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
Set-Location $rootDir

Write-Host "Checking WinBinder source files in: $rootDir" -ForegroundColor Gray
Write-Host ""

# Check for direct zval access
Write-Host "Checking for remaining direct zval access..." -ForegroundColor Yellow
$zval_issues = Select-String -Path "$rootDir/phpwb*.c" -Pattern "->value\.(lval|str\.val|dval)" | 
    Where-Object { $_.Line -notmatch "^\s*//" -and $_.Line -notmatch "^\s*\*" }

# Filter out commented code blocks
$active_issues = @()
foreach ($issue in $zval_issues) {
    $file_content = Get-Content $issue.Path
    $line_num = $issue.LineNumber - 1
    
    # Check if we're in a comment block
    $in_comment = $false
    for ($i = 0; $i -lt $line_num; $i++) {
        if ($file_content[$i] -match "^/\*" -and $file_content[$i] -notmatch "\*/") {
            $in_comment = $true
        }
        if ($file_content[$i] -match "\*/" -and $in_comment) {
            $in_comment = $false
        }
    }
    
    if (-not $in_comment) {
        $active_issues += $issue
    }
}

if ($active_issues.Count -eq 0) {
    Write-Host "✓ No direct zval access found in active code" -ForegroundColor Green
    if ($zval_issues.Count -gt 0) {
        Write-Host "  (Found $($zval_issues.Count) occurrences in commented code - OK)" -ForegroundColor Gray
    }
} else {
    Write-Host "✗ Direct zval access still present in:" -ForegroundColor Red
    $active_issues | ForEach-Object { Write-Host "  $($_.Filename):$($_.LineNumber)" -ForegroundColor Red }
}

Write-Host ""

# Check for TSRMLS issues
Write-Host "Checking for TSRMLS issues..." -ForegroundColor Yellow
$tsrmls_issues = Select-String -Path "$rootDir/phpwb*.c" -Pattern "get_active_function_name\(TSRMLS_C\)" | 
    Where-Object { $_.Line -notmatch "^\s*//" -and $_.Line -notmatch "^\s*\*" }

# Filter out commented code blocks
$active_tsrmls = @()
foreach ($issue in $tsrmls_issues) {
    $file_content = Get-Content $issue.Path
    $line_num = $issue.LineNumber - 1
    
    # Check if we're in a comment block
    $in_comment = $false
    for ($i = 0; $i -lt $line_num; $i++) {
        if ($file_content[$i] -match "^/\*" -and $file_content[$i] -notmatch "\*/") {
            $in_comment = $true
        }
        if ($file_content[$i] -match "\*/" -and $in_comment) {
            $in_comment = $false
        }
    }
    
    if (-not $in_comment) {
        $active_tsrmls += $issue
    }
}

if ($active_tsrmls.Count -eq 0) {
    Write-Host "✓ TSRMLS usage fixed in active code" -ForegroundColor Green
    if ($tsrmls_issues.Count -gt 0) {
        Write-Host "  (Found $($tsrmls_issues.Count) occurrences in commented code - OK)" -ForegroundColor Gray
    }
} else {
    Write-Host "✗ TSRMLS issues found in:" -ForegroundColor Red
    $active_tsrmls | ForEach-Object { Write-Host "  $($_.Filename):$($_.LineNumber)" -ForegroundColor Red }
}

Write-Host ""

# Check documentation files
Write-Host "Checking documentation files..." -ForegroundColor Yellow
$docsDir = Join-Path $rootDir "docs/php83-compatibility"
$docs = @(
    "PHP_8.3_COMPATIBILITY_FIXES.md",
    "TEST_CHECKLIST.md",
    "MIGRATION_GUIDE.md",
    "README_PHP83_FIXES.md",
    "README.md"
)

foreach ($doc in $docs) {
    $docPath = Join-Path $docsDir $doc
    if (Test-Path $docPath) {
        Write-Host "✓ $doc" -ForegroundColor Green
    } else {
        Write-Host "✗ $doc missing" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "=== Summary ===" -ForegroundColor Cyan

if ($active_issues.Count -eq 0 -and $active_tsrmls.Count -eq 0) {
    Write-Host "✓ All PHP 8.3.29 compatibility fixes have been applied successfully!" -ForegroundColor Green
    Write-Host ""
    Write-Host "Next steps:" -ForegroundColor Yellow
    Write-Host "1. Rebuild the extension using your build environment"
    Write-Host "2. Run the tests in TEST_CHECKLIST.md"
    Write-Host "3. Deploy and monitor for issues"
} else {
    Write-Host "✗ Some issues remain. Please review the output above." -ForegroundColor Red
}

Write-Host ""
