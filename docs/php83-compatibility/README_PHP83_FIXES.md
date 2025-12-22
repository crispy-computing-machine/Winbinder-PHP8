# WinBinder PHP 8.3.29 Compatibility Fixes - Summary

## Quick Start

Your WinBinder extension has been updated for PHP 8.3.29 compatibility. The main issue causing problems when upgrading from PHP 8.2.28 has been fixed.

## What Was Wrong?

In PHP 8.3, the internal structure of `zval` (PHP's value container) changed. Direct access to fields like `zval->value.lval` is no longer allowed and causes crashes or undefined behavior.

## What Was Fixed?

All direct zval access has been replaced with proper accessor macros:

- `source->value.lval` → `Z_LVAL_P(source)`
- `source->value.str.val` → `Z_STRVAL_P(source)`
- `source->value.dval` → `Z_DVAL_P(source)`

Additionally, deprecated TSRMLS parameters were removed from function calls.

## Files Changed

- ✅ phpwb_window.c
- ✅ phpwb_winsys.c
- ✅ phpwb_control.c
- ✅ phpwb_control_listview.c
- ✅ phpwb_wb_lib.c

## Next Steps

1. **Rebuild the extension** using your build environment
2. **Test thoroughly** using the provided test checklist
3. **Report any issues** you encounter

## Documentation Location

All PHP 8.3 compatibility documentation is located in `docs/php83-compatibility/`:

1. **PHP_8.3_COMPATIBILITY_FIXES.md** - Detailed list of all changes
2. **TEST_CHECKLIST.md** - Comprehensive testing guide
3. **MIGRATION_GUIDE.md** - Developer reference for understanding the changes
4. **verify_fixes.ps1** - Verification script

## Verification

Run the verification script from the root directory:

```bash
# From root directory
pwsh verify-php83-fixes.ps1

# Or directly
pwsh docs/php83-compatibility/verify_fixes.ps1
```

## Quick Test

After rebuilding, run this simple test:

```php
<?php
// Test basic functionality
if (!extension_loaded('winbinder')) {
    die("WinBinder extension not loaded!\n");
}

echo "PHP Version: " . PHP_VERSION . "\n";
echo "WinBinder loaded successfully!\n";

// Test window creation
$window = wb_create_window(null, AppWindow, "Test Window", 100, 100, 400, 300);
if ($window) {
    echo "Window created successfully!\n";
    wb_destroy_window($window);
    echo "Window destroyed successfully!\n";
} else {
    echo "ERROR: Failed to create window!\n";
}

// Test registry operations (critical for zval handling)
$test_key = "HKEY_CURRENT_USER";
$test_subkey = "Software\\WinBinderTest";

// Test with integer
wb_set_registry_key($test_key, $test_subkey, "TestInt", 12345);
$val = wb_get_registry_key($test_key, $test_subkey, "TestInt");
echo "Integer test: " . ($val == "12345" ? "PASS" : "FAIL") . "\n";

// Test with string
wb_set_registry_key($test_key, $test_subkey, "TestString", "Hello PHP 8.3");
$val = wb_get_registry_key($test_key, $test_subkey, "TestString");
echo "String test: " . ($val == "Hello PHP 8.3" ? "PASS" : "FAIL") . "\n";

// Test with double
wb_set_registry_key($test_key, $test_subkey, "TestDouble", 3.14159);
$val = wb_get_registry_key($test_key, $test_subkey, "TestDouble");
echo "Double test: " . (abs(floatval($val) - 3.14159) < 0.0001 ? "PASS" : "FAIL") . "\n";

echo "\nAll basic tests completed!\n";
?>
```

## Build Instructions

### Windows with Visual Studio

```batch
REM Set up PHP SDK environment
cd C:\php-sdk
phpsdk-vs16-x64.bat

REM Navigate to extension directory
cd E:\Bearsampp-development\Winbinder-PHP8

REM Configure and build
phpize
configure --enable-winbinder
nmake

REM The compiled extension will be in:
REM x64\Release_TS\php_winbinder.dll (Thread-Safe)
REM or
REM x64\Release\php_winbinder.dll (Non-Thread-Safe)
```

### Copy to PHP Extensions Directory

```batch
copy x64\Release_TS\php_winbinder.dll C:\php\ext\
```

### Enable in php.ini

```ini
extension=winbinder
```

## Troubleshooting

### Extension Won't Load
- Check PHP version: `php -v` (must be 8.3.x)
- Check extension path in php.ini
- Check for error messages: `php -m`

### Crashes or Segfaults
- Ensure you rebuilt the extension (don't use old DLL)
- Check PHP error log
- Run with debugging enabled

### Functions Not Working
- Review TEST_CHECKLIST.md
- Check specific function usage
- Verify parameter types

## Technical Details

### Why This Happened
PHP 8.3 introduced internal changes to improve:
- Type safety
- Memory management
- Performance optimizations
- Future compatibility

### Impact on Your Code
- **PHP code**: No changes needed
- **C extension code**: Must use accessor macros
- **Performance**: No performance impact (macros are inlined)

## Support and Resources

### Documentation Files
- `PHP_8.3_COMPATIBILITY_FIXES.md` - What was changed
- `TEST_CHECKLIST.md` - How to test
- `MIGRATION_GUIDE.md` - Developer reference

### External Resources
- [PHP 8.3 Release Notes](https://www.php.net/releases/8.3/en.php)
- [PHP Internals Documentation](https://www.phpinternalsbook.com/)

## Version Information

- **PHP Version**: 8.3.29
- **Previous Working Version**: 8.2.28
- **Fix Date**: 2024
- **Status**: ✅ Fixed and Ready for Testing

## Checklist

- [x] Identified compatibility issues
- [x] Fixed all zval direct access
- [x] Fixed TSRMLS usage
- [x] Created documentation
- [x] Created test checklist
- [ ] Rebuild extension
- [ ] Run tests
- [ ] Deploy to production

## Questions?

If you encounter any issues:
1. Check the error logs
2. Review the test checklist
3. Consult the migration guide
4. Check PHP version compatibility

---

**Important**: Make sure to rebuild the extension after applying these fixes. Simply copying the old DLL will not work!
