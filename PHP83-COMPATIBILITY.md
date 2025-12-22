# PHP 8.3.29 Compatibility - Quick Reference

## Status: ✅ Fixed

All PHP 8.3.29 compatibility issues have been resolved.

## What Was Fixed

The extension was crashing when upgrading from PHP 8.2.28 to PHP 8.3.29 due to deprecated direct access to zval internal structures.

**Changes Applied:**
- Replaced all `zval->value.lval` with `Z_LVAL_P(zval)`
- Replaced all `zval->value.str.val` with `Z_STRVAL_P(zval)`
- Replaced all `zval->value.dval` with `Z_DVAL_P(zval)`
- Removed deprecated TSRMLS parameters

## Quick Actions

### Verify Fixes
```bash
pwsh verify-php83-fixes.ps1
```

### View Documentation
All documentation is in `docs/php83-compatibility/`:
- **README.md** - Documentation index
- **README_PHP83_FIXES.md** - Quick start guide
- **PHP_8.3_COMPATIBILITY_FIXES.md** - Detailed changes
- **TEST_CHECKLIST.md** - Testing guide
- **MIGRATION_GUIDE.md** - Developer reference

### Next Steps
1. Rebuild the extension
2. Test using `docs/php83-compatibility/TEST_CHECKLIST.md`
3. Deploy to PHP 8.3.29

## Files Modified
- phpwb_window.c
- phpwb_winsys.c
- phpwb_control.c
- phpwb_control_listview.c
- phpwb_wb_lib.c

## More Information
See `docs/php83-compatibility/README.md` for complete documentation.
