# PHP 8.3.29 Compatibility Fixes for WinBinder

## Summary
Fixed compatibility issues when upgrading from PHP 8.2.28 to PHP 8.3.29. The main issue was the deprecated direct access to zval internal structures.

## Changes Made

### 1. Fixed Direct zval Access (Critical Issue)
In PHP 8.3, direct access to `zval->value.lval`, `zval->value.str`, and `zval->value.dval` is no longer supported. These must be accessed through macros.

#### Files Modified:
- **phpwb_window.c**
  - Changed `source->value.lval` to `Z_LVAL_P(source)` (3 occurrences)
  - Changed `zparm->value.lval` to `Z_LVAL_P(zparm)` (2 occurrences)

- **phpwb_winsys.c**
  - Changed `source->value.lval` to `Z_LVAL_P(source)` (2 occurrences)
  - Changed `source->value.dval` to `Z_DVAL_P(source)` (1 occurrence)

- **phpwb_control.c**
  - Changed `source->value.lval` to `Z_LVAL_P(source)` (1 occurrence)
  - Changed `zindex->value.lval` to `Z_LVAL_P(zindex)` (2 occurrences)
  - Changed `zcaption->value.str.val` to `Z_STRVAL_P(zcaption)` (3 occurrences)
  - Changed `zcaption->value.lval` to `Z_LVAL_P(zcaption)` (1 occurrence)

- **phpwb_control_listview.c**
  - Changed `(entry)->value.lval` to `Z_LVAL_P(entry)` (1 occurrence)

### 2. Fixed TSRMLS Usage
The `TSRMLS_C` parameter is no longer needed in PHP 8.x for `get_active_function_name()`.

#### Files Modified:
- **phpwb_control_listview.c**
  - Changed `get_active_function_name(TSRMLS_C)` to `get_active_function_name()` (2 occurrences)

- **phpwb_wb_lib.c**
  - Changed `get_active_function_name(TSRMLS_C)` to `get_active_function_name()` (1 occurrence)

## Technical Details

### Why These Changes Were Necessary
PHP 8.3 introduced stricter type safety and internal structure changes. Direct access to zval internals was deprecated to:
1. Improve type safety
2. Allow for future internal optimizations
3. Prevent undefined behavior from direct memory access

### Proper Macros to Use
- `Z_LVAL_P(zval_ptr)` - Access long value
- `Z_DVAL_P(zval_ptr)` - Access double value
- `Z_STRVAL_P(zval_ptr)` - Access string value
- `Z_STRLEN_P(zval_ptr)` - Access string length
- `Z_TYPE_P(zval_ptr)` - Access type

## Testing Recommendations
1. Test all window creation and manipulation functions
2. Test registry key operations (uses both long and double values)
3. Test control creation with various parameter types
4. Test listview operations
5. Test cursor and image handling

## Build Instructions
After applying these fixes, rebuild the extension using your standard build process for PHP extensions on Windows.

## Notes
- All commented-out code containing old-style access was left as-is since it's not compiled
- The changes maintain backward compatibility with the API
- No functional changes were made, only internal implementation updates

## Date
Applied: 2024
PHP Version Target: 8.3.29
Previous Working Version: 8.2.28
