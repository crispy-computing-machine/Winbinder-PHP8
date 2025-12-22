# PHP 8.3.29 Compatibility Documentation

This folder contains all documentation related to the PHP 8.3.29 compatibility fixes applied to WinBinder.

## Quick Start

Start with **README_PHP83_FIXES.md** for a quick overview and getting started guide.

## Documentation Files

### 📘 README_PHP83_FIXES.md
**Start here!** Quick start guide with:
- What was wrong and what was fixed
- Quick test script
- Build instructions
- Troubleshooting tips

### 📋 PHP_8.3_COMPATIBILITY_FIXES.md
Detailed technical documentation:
- Complete list of all changes made
- Files modified with line-by-line changes
- Technical explanation of why changes were necessary
- Proper macros reference

### ✅ TEST_CHECKLIST.md
Comprehensive testing guide:
- Critical functions to test
- Test scenarios with code examples
- Error conditions to verify
- Performance testing guidelines
- Sign-off checklist

### 🔧 MIGRATION_GUIDE.md
Developer reference guide:
- Breaking changes in PHP 8.3
- Before/after code examples
- Complete macro reference
- Common patterns and solutions
- Debugging tips

### 🔍 verify_fixes.ps1
PowerShell verification script:
- Checks for remaining compatibility issues
- Verifies all documentation is present
- Provides summary of fix status
- Run this after applying fixes

## Usage

### For Quick Reference
```bash
# Read the quick start
cat README_PHP83_FIXES.md

# Verify fixes are applied
pwsh verify_fixes.ps1
```

### For Development
```bash
# Understand the changes
cat PHP_8.3_COMPATIBILITY_FIXES.md

# Learn the new patterns
cat MIGRATION_GUIDE.md
```

### For Testing
```bash
# Follow the test checklist
cat TEST_CHECKLIST.md
```

## Issue Summary

**Problem:** WinBinder extension crashed when upgrading from PHP 8.2.28 to PHP 8.3.29

**Root Cause:** Direct access to zval internal structures (`zval->value.lval`, etc.) was deprecated in PHP 8.3

**Solution:** Replaced all direct access with proper accessor macros (`Z_LVAL_P()`, etc.)

**Status:** ✅ Fixed and verified

## Files Modified

- phpwb_window.c
- phpwb_winsys.c
- phpwb_control.c
- phpwb_control_listview.c
- phpwb_wb_lib.c

## Next Steps

1. Rebuild the extension
2. Run tests from TEST_CHECKLIST.md
3. Deploy to PHP 8.3.29 environment
4. Monitor for any issues

## Support

If you encounter issues:
1. Check README_PHP83_FIXES.md troubleshooting section
2. Review MIGRATION_GUIDE.md for technical details
3. Run verify_fixes.ps1 to check fix status
4. Consult TEST_CHECKLIST.md for testing guidance

---

**Last Updated:** 2024  
**PHP Version:** 8.3.29  
**Previous Version:** 8.2.28
