# PHP 8.3.29 Compatibility Test Checklist

## Critical Functions to Test

### Window Functions (phpwb_window.c)
- [ ] `wb_create_window()` - Test with various parameter types
- [ ] `wb_get_size()` - Test with window handles, image handles, and file paths
- [ ] `wb_set_size()` - Test with both single values and arrays (ListView columns)
- [ ] `wb_get_position()` - Test window position retrieval
- [ ] `wb_set_position()` - Test window positioning

### System Functions (phpwb_winsys.c)
- [ ] `wb_set_cursor()` - Test with both integer handles and string cursor names
- [ ] `wb_play_sound()` - Test with both integer (system sound) and string (file path)
- [ ] `wb_set_registry_key()` - **CRITICAL** - Test with:
  - [ ] NULL values
  - [ ] Integer/Long values
  - [ ] Boolean values (TRUE/FALSE)
  - [ ] Double/Float values
  - [ ] String values
- [ ] `wb_get_registry_key()` - Test reading various value types

### Control Functions (phpwb_control.c)
- [ ] `wb_set_image()` - Test with both integer handles and string paths
- [ ] `wb_set_item_image()` - Test with integer image indices
- [ ] `wb_set_text()` - Test with both string and array captions
- [ ] `wb_create_control()` - Test with various caption types

### ListView Functions (phpwb_control_listview.c)
- [ ] `wb_create_listview_column()` - Test column width handling
- [ ] `wb_set_listview_item_text()` - Test text setting
- [ ] `wb_get_listview_text()` - Test text retrieval

## Test Scenarios

### Basic Functionality
1. Create a simple window
2. Add controls (buttons, labels, etc.)
3. Set and get control properties
4. Handle window events

### Data Type Handling
1. Pass integers where expected
2. Pass strings where expected
3. Pass arrays where expected
4. Test NULL/empty values
5. Test boolean values (TRUE/FALSE)
6. Test floating-point numbers

### Registry Operations
```php
// Test all data types
wb_set_registry_key("HKEY_CURRENT_USER", "Software\\Test", "IntValue", 12345);
wb_set_registry_key("HKEY_CURRENT_USER", "Software\\Test", "BoolValue", true);
wb_set_registry_key("HKEY_CURRENT_USER", "Software\\Test", "FloatValue", 3.14159);
wb_set_registry_key("HKEY_CURRENT_USER", "Software\\Test", "StringValue", "Hello");
wb_set_registry_key("HKEY_CURRENT_USER", "Software\\Test", "NullValue", null);

// Read them back
$int_val = wb_get_registry_key("HKEY_CURRENT_USER", "Software\\Test", "IntValue");
$bool_val = wb_get_registry_key("HKEY_CURRENT_USER", "Software\\Test", "BoolValue");
$float_val = wb_get_registry_key("HKEY_CURRENT_USER", "Software\\Test", "FloatValue");
$string_val = wb_get_registry_key("HKEY_CURRENT_USER", "Software\\Test", "StringValue");
```

### Image/Cursor Handling
```php
// Test with file paths
$img = wb_load_image("path/to/image.bmp");
wb_set_image($control, $img);

// Test with cursor names
wb_set_cursor($window, "arrow");
wb_set_cursor($window, "hand");

// Test with handles
wb_set_cursor($window, $cursor_handle);
```

### ListView Operations
```php
// Create listview with columns
$listview = wb_create_control($window, ListView, ...);
wb_create_listview_column($listview, 0, "Column 1", 100, WBC_LEFT);
wb_create_listview_column($listview, 1, "Column 2", 150, WBC_CENTER);

// Set column widths
wb_set_size($listview, [100, 150, 200]);

// Get column widths
$widths = wb_get_size($listview, true);
```

## Error Conditions to Test

1. **Invalid Handles**
   - Pass 0 or NULL where handle expected
   - Pass invalid handle values

2. **Type Mismatches**
   - Pass wrong type for parameters
   - Pass array where scalar expected
   - Pass scalar where array expected

3. **Boundary Conditions**
   - Very large integers
   - Very small/large floats
   - Empty strings
   - Very long strings

## Known Issues from PHP 8.2 to 8.3

### What Was Fixed
- Direct zval access (`->value.lval`, `->value.str.val`, `->value.dval`)
- TSRMLS parameter usage in function calls

### What to Watch For
- Memory leaks (use memory profiling tools)
- Segmentation faults (especially with invalid handles)
- Type coercion issues
- String encoding problems (UTF-8 handling)

## Regression Testing

Run all existing test suites that worked in PHP 8.2.28 to ensure:
1. No functionality was broken
2. All return values are correct
3. Error handling still works
4. Memory management is correct

## Performance Testing

Compare performance between PHP 8.2.28 and 8.3.29:
1. Window creation speed
2. Control manipulation speed
3. Image loading/handling
4. Registry operations

## Sign-off

- [ ] All critical functions tested
- [ ] All data types tested
- [ ] Error conditions handled correctly
- [ ] No memory leaks detected
- [ ] Performance acceptable
- [ ] Regression tests passed

**Tested by:** _______________
**Date:** _______________
**PHP Version:** 8.3.29
**Notes:** _______________
