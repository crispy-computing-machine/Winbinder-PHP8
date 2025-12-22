# WinBinder PHP 8.2 to 8.3 Migration Guide

## Overview
This guide explains the changes needed to migrate WinBinder from PHP 8.2.28 to PHP 8.3.29.

## Breaking Changes in PHP 8.3

### 1. Direct zval Access Removed
**Problem:** Direct access to zval internal structures is no longer allowed.

**Old Code (PHP 8.2):**
```c
if (source->value.lval) {
    handle = (HANDLE)source->value.lval;
}

char *str = source->value.str.val;
double num = source->value.dval;
```

**New Code (PHP 8.3):**
```c
if (Z_LVAL_P(source)) {
    handle = (HANDLE)Z_LVAL_P(source);
}

char *str = Z_STRVAL_P(source);
double num = Z_DVAL_P(source);
```

### 2. TSRMLS Parameter Removed
**Problem:** Thread-safe resource manager local storage (TSRMLS) parameters are no longer needed.

**Old Code (PHP 8.2):**
```c
get_active_function_name(TSRMLS_C)
```

**New Code (PHP 8.3):**
```c
get_active_function_name()
```

## Complete Macro Reference

### Accessing zval Values

| Old Access | New Macro | Description |
|------------|-----------|-------------|
| `zv->value.lval` | `Z_LVAL_P(zv)` | Long/integer value |
| `zv->value.dval` | `Z_DVAL_P(zv)` | Double/float value |
| `zv->value.str.val` | `Z_STRVAL_P(zv)` | String value (char*) |
| `zv->value.str.len` | `Z_STRLEN_P(zv)` | String length |
| `zv->type` | `Z_TYPE_P(zv)` | Value type |

### Type Checking

```c
// Check if zval is a specific type
if (Z_TYPE_P(zv) == IS_LONG) { ... }
if (Z_TYPE_P(zv) == IS_STRING) { ... }
if (Z_TYPE_P(zv) == IS_DOUBLE) { ... }
if (Z_TYPE_P(zv) == IS_ARRAY) { ... }
if (Z_TYPE_P(zv) == IS_NULL) { ... }
if (Z_TYPE_P(zv) == IS_TRUE) { ... }
if (Z_TYPE_P(zv) == IS_FALSE) { ... }
```

### Working with Arrays

```c
// Get array hash table
HashTable *ht = Z_ARRVAL_P(zarray);

// Get number of elements
int count = zend_hash_num_elements(ht);

// Iterate through array
zval *entry;
ZEND_HASH_FOREACH_VAL(ht, entry) {
    // Process entry
    long val = Z_LVAL_P(entry);
} ZEND_HASH_FOREACH_END();
```

## Common Patterns

### Pattern 1: Type-Based Dispatch

**Old:**
```c
switch (source->type) {
    case IS_LONG:
        handle = (HANDLE)source->value.lval;
        break;
    case IS_STRING:
        path = source->value.str.val;
        break;
}
```

**New:**
```c
switch (Z_TYPE_P(source)) {
    case IS_LONG:
        handle = (HANDLE)Z_LVAL_P(source);
        break;
    case IS_STRING:
        path = Z_STRVAL_P(source);
        break;
}
```

### Pattern 2: Null Checking

**Old:**
```c
if (!source->value.lval) {
    RETURN_NULL();
}
```

**New:**
```c
if (!Z_LVAL_P(source)) {
    RETURN_NULL();
}
```

### Pattern 3: String Operations

**Old:**
```c
char *str = source->value.str.val;
int len = source->value.str.len;
```

**New:**
```c
char *str = Z_STRVAL_P(source);
size_t len = Z_STRLEN_P(source);
```

## Files Modified in This Migration

1. **phpwb_window.c** - Window creation and sizing
2. **phpwb_winsys.c** - System functions, registry, cursors
3. **phpwb_control.c** - Control creation and manipulation
4. **phpwb_control_listview.c** - ListView operations
5. **phpwb_wb_lib.c** - Library functions
6. **phpwb_control_listview.c** - ListView-specific operations

## Testing Strategy

### Unit Tests
Create tests for each modified function:
```php
// Test integer handling
$result = wb_set_registry_key("HKCU", "Software\\Test", "Value", 123);
assert($result === true);

// Test string handling
$result = wb_set_registry_key("HKCU", "Software\\Test", "Value", "test");
assert($result === true);

// Test double handling
$result = wb_set_registry_key("HKCU", "Software\\Test", "Value", 3.14);
assert($result === true);
```

### Integration Tests
Test complete workflows:
```php
// Create window
$window = wb_create_window(null, AppWindow, "Test", 100, 100, 400, 300);

// Create control
$button = wb_create_control($window, PushButton, "Click Me", 10, 10, 100, 30);

// Set properties
wb_set_text($button, "New Text");
wb_set_enabled($button, true);

// Clean up
wb_destroy_window($window);
```

## Debugging Tips

### Enable Error Reporting
```php
error_reporting(E_ALL);
ini_set('display_errors', 1);
```

### Check PHP Version
```php
echo "PHP Version: " . PHP_VERSION . "\n";
echo "Extension loaded: " . (extension_loaded('winbinder') ? 'Yes' : 'No') . "\n";
```

### Memory Debugging
Use Valgrind or similar tools to check for:
- Memory leaks
- Invalid memory access
- Use-after-free errors

## Common Errors and Solutions

### Error: Segmentation Fault
**Cause:** Accessing freed memory or invalid pointers
**Solution:** Check all zval accesses use proper macros

### Error: Type Mismatch
**Cause:** Passing wrong type to function
**Solution:** Add type checking before accessing values

### Error: Undefined Symbol
**Cause:** Using removed TSRMLS parameters
**Solution:** Remove TSRMLS_C from function calls

## Performance Considerations

The new macro-based access is:
- **Equally fast** - Macros are inlined at compile time
- **Type-safe** - Better error checking
- **Future-proof** - Compatible with future PHP versions

## Additional Resources

- [PHP 8.3 Migration Guide](https://www.php.net/manual/en/migration83.php)
- [PHP Internals Book](https://www.phpinternalsbook.com/)
- [Zend API Documentation](https://www.php.net/manual/en/internals2.php)

## Support

If you encounter issues:
1. Check this migration guide
2. Review the test checklist
3. Examine the compatibility fixes document
4. Check PHP error logs
5. Use debugging tools (gdb, Valgrind)

## Version History

- **v1.0** - Initial migration from PHP 8.2.28 to 8.3.29
- **Date:** 2024
- **Status:** Tested and verified
