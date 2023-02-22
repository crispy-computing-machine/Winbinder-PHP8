// 					See: https://github.com/php/php-src/blob/master/Zend/zend_API.h

// ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(name, return_reference, required_num_args, MAY_BE_STRING|MAY_BE_BOOL)
// ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(name, return_reference, required_num_args, type, allow_null)

//		ZEND_ARG_TYPE_INFO(pass_by_ref, name, type_hint, allow_null)
// 		ZEND_ARG_TYPE_MASK(pass_by_ref, name, MAY_BE_STRING|MAY_BE_BOOL, default_value)

//		ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(pass_by_ref, name, type_hint, allow_null, default_value)
// 		ZEND_ARG_INFO_WITH_DEFAULT_VALUE(pass_by_ref, name, default_value)

// ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_get_address, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, source, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_load_library, 0, 1, MAY_BE_LONG|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, lib, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_get_function_address, 0, 1, MAY_BE_LONG|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, fun, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, hlib, IS_LONG, 1, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_call_function, 0, 1, MAY_BE_LONG|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, addr, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, array, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_main_loop, 0, 0, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_find_file, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_message_box, 0, 4, MAY_BE_LONG|MAY_BE_BOOL|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, msg, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, title, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, style, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_play_sound, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_MASK(0, source, MAY_BE_STRING|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, command, IS_STRING, 1, "\"play\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_stop_sound, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_MASK(0, command, MAY_BE_STRING|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_exec, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pgm, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, param, MAY_BE_STRING|MAY_BE_NULL, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, show, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_get_system_info, 0, 0, IS_MIXED, 0)
	ZEND_ARG_TYPE_MASK(0, config, MAY_BE_STRING|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_get_registry_key, 0, 2, MAY_BE_STRING|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, subkey, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, entry, MAY_BE_STRING|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_registry_key, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, subkey, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, entry, MAY_BE_STRING|MAY_BE_NULL, "null")
	ZEND_ARG_TYPE_MASK(0, source, MAY_BE_DOUBLE|MAY_BE_BOOL|MAY_BE_STRING|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_create_timer, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, id, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, ms, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_wait, 0, 0, MAY_BE_NULL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pwbo, IS_LONG, 1, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pause, IS_LONG, 1, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 1, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_is_obj, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_destroy_timer, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, mids, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_accel_table, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, zarray, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_get_clipboard, 0, 0, MAY_BE_NULL|MAY_BE_STRING)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_clipboard, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, clip, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_empty_clipboard, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_get_mouse_pos, 0, 0, MAY_BE_BOOL|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_is_mouse_over, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, left, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, top, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, right, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, bottom, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_load_image, 0, 1, MAY_BE_NULL|MAY_BE_LONG)

	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, index, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, param, MAY_BE_LONG|MAY_BE_NULL, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_save_image, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, hbm, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_create_image, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, w, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, h, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, bmi, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, bits, MAY_BE_LONG|MAY_BE_NULL, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_create_mask, 0, 2, MAY_BE_NULL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, hbm, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, c, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_destroy_image, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, hbm, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_get_image_data, 0, 1, MAY_BE_NULL|MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO(0, hbm, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, compress4to3, MAY_BE_LONG|MAY_BE_NULL, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_get_pixel, 0, 3, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, handle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_draw_point, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, handle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_draw_line, 0, 6,_IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, handle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, x0, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y0, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, x1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, linewidth, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, linestyle, MAY_BE_LONG|MAY_BE_NULL, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_draw_rect, 0, 6,_IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, handle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, filled, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, linewidth, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, linestyle, MAY_BE_LONG|MAY_BE_NULL, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_draw_ellipse, 0, 6,_IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, handle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, color, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, filled, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, linewidth, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, linestyle, MAY_BE_LONG|MAY_BE_NULL, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_draw_text, 0, 4,_IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, handle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, text, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, width, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, height, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, nfont, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, flags, MAY_BE_LONG|MAY_BE_NULL, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_draw_image, 0, 2,_IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, handle, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, hbm, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, x, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, y, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, w, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, h, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, transpcolor, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, cx, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, cy, MAY_BE_LONG|MAY_BE_NULL, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_destroy_control, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_get_value, 0, 1, MAY_BE_NULL|MAY_BE_BOOL|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, item, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, subitem, MAY_BE_LONG|MAY_BE_NULL, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_refresh, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, now, MAY_BE_BOOL|MAY_BE_LONG, "true")
	
	ZEND_ARG_TYPE_MASK(0, x, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, y, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, width, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, height, MAY_BE_LONG|MAY_BE_NULL, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_enabled, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, state, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_image, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, source, MAY_BE_LONG|MAY_BE_STRING, "0")
	ZEND_ARG_TYPE_MASK(0, trcolor, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, index, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, param, MAY_BE_LONG|MAY_BE_NULL, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_item_image, 0, 2,_IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, zindex, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, item, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, subitem, MAY_BE_LONG|MAY_BE_NULL, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_create_control, 0, 2,  MAY_BE_NULL|MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, pwboparent, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, wbclass, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, zcaption, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, x, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, y, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, w, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, h, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, id, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, style, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, param, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, ntab, MAY_BE_LONG|MAY_BE_NULL, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_create_item, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, param, MAY_BE_LONG|MAY_BE_NULL, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_create_statusbar_items, 0, 2, MAY_BE_NULL|MAY_BE_BOOL|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, zitems, IS_ARRAY, 0)
	ZEND_ARG_TYPE_MASK(0, clear, MAY_BE_LONG|MAY_BE_NULL, "0")
	ZEND_ARG_TYPE_MASK(0, param, MAY_BE_LONG|MAY_BE_NULL, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_get_text, 0, 1, MAY_BE_NULL|MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, index, MAY_BE_LONG|MAY_BE_NULL, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_text, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, zcaption, MAY_BE_STRING|MAY_BE_ARRAY, 0)
	ZEND_ARG_TYPE_MASK(0, item, MAY_BE_LONG|MAY_BE_NULL, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_delete_items, 0, 1, MAY_BE_NULL|MAY_BE_BOOL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, zitems, MAY_BE_LONG|MAY_BE_ARRAY, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_get_class, 0, 1, MAY_BE_NULL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_get_control, 0, 2, MAY_BE_NULL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, pwboparent, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, id, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_get_enabled, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_get_focus, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_get_id, 0, 1, MAY_BE_NULL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_get_item_count, 0, 1, MAY_BE_NULL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_get_parent, 0, 2, MAY_BE_NULL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, item, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_get_selected, 0, 1, MAY_BE_NULL|MAY_BE_ARRAY|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_get_state, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, item, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_get_visible, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_cursor, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, zitems, MAY_BE_LONG|MAY_BE_STRING, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_focus, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_handler, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, zparam, MAY_BE_ARRAY|MAY_BE_STRING, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_location, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, location, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_range, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, min, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, max, MAY_BE_NULL|MAY_BE_LONG, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_state, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, item, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, state, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_style, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, style, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, value, MAY_BE_NULL|MAY_BE_LONG, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_visible, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, b, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_sort, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, ascending, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, subitem, MAY_BE_NULL|MAY_BE_LONG, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_select_tab, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, ntab, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_value, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, item, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, subitem, MAY_BE_NULL|MAY_BE_LONG, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_create_listview_item, 0, 4, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, nitem, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, nimage, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_listview_item_checked, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, item, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, checked, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_get_listview_item_checked, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, item, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_listview_item_text, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, nitem, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, sub, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_get_listview_text, 0, 1, MAY_BE_NULL|MAY_BE_STRING|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, nitem, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, ncol, MAY_BE_NULL|MAY_BE_LONG, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_get_listview_columns, 0, 1, MAY_BE_NULL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_create_listview_column, 0, 5, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, ncol, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, w, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, align, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_clear_listview_columns, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_select_listview_item, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, nitem, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, state, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_select_all_listview_items, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, state, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_create_menu, 0, 2, MAY_BE_NULL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, pwboParent, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, zarray, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_get_menu_item_checked, 0, 2,_IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, id, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_menu_item_checked, 0, 3,_IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, id, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, b, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_menu_item_selected, 0, 3,_IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, id, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, state, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_menu_item_image, 0, 3,_IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, id, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, handle, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_create_toolbar, 0, 2, MAY_BE_NULL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, pwboParent, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, zarray, IS_ARRAY, 0)
	ZEND_ARG_TYPE_MASK(0, width, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, height, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, image, MAY_BE_NULL|MAY_BE_STRING, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_create_treeview_item, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, zparam, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, where, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, img1, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, img2, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, insertiontype, MAY_BE_NULL|MAY_BE_LONG, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_treeview_item_selected, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, item, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_treeview_item_text, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, item, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_treeview_item_value, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, item, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, zparam, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_get_treeview_item_text, 0, 2, MAY_BE_NULL|MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, item, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_get_level, 0, 2, MAY_BE_NULL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, item, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_create_font, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, color, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, flags, MAY_BE_NULL|MAY_BE_LONG, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_destroy_font, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_MASK(0, font, MAY_BE_NULL|MAY_BE_LONG, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_font, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, nfont, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, redraw, MAY_BE_NULL|MAY_BE_LONG, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_send_message, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, msg, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, w, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, l, MAY_BE_NULL|MAY_BE_LONG, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_peek, 0, 1, MAY_BE_NULL|MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO(0, address, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, bytes, MAY_BE_NULL|MAY_BE_LONG, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_poke, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, address, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, contents, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, bytes, MAY_BE_NULL|MAY_BE_LONG, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_release_library, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, hlib, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_get_midi_callback, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_get_enum_callback, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_get_hook_callback, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_destroy_window, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_get_size, 0, 1, MAY_BE_NULL|MAY_BE_ARRAY|MAY_BE_LONG)
	ZEND_ARG_TYPE_MASK(0, source, MAY_BE_LONG|MAY_BE_STRING, "0")
	ZEND_ARG_TYPE_MASK(0, lparam, MAY_BE_NULL|MAY_BE_LONG, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_size, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_MASK(0, pwbo, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, zparm, MAY_BE_ARRAY|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, h, MAY_BE_NULL|MAY_BE_LONG, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_position, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, x, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, y, MAY_BE_NULL|MAY_BE_LONG, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_get_position, 0, 1, MAY_BE_NULL|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, clientarea, MAY_BE_NULL|MAY_BE_LONG, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_create_window, 0, 2, MAY_BE_NULL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, pwboparent, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, wbclass, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, zcaption, MAY_BE_STRING|MAY_BE_ARRAY, "null")
	ZEND_ARG_TYPE_MASK(0, x, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, y, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, w, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, h, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, style, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, lparam, MAY_BE_NULL|MAY_BE_LONG, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_get_instance, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, caption, IS_STRING, 0)
	ZEND_ARG_TYPE_MASK(0, bringtofront, MAY_BE_NULL|MAY_BE_BOOL, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_get_item_list, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, pwboparent, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_set_area, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, pwbo, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, x, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, y, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, w, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, h, MAY_BE_NULL|MAY_BE_LONG, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_sys_dlg_path, 0, 1, IS_STRING)
	ZEND_ARG_TYPE_INFO(0, pwboParent, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, title, MAY_BE_NULL|MAY_BE_STRING, "\"\"")
	ZEND_ARG_TYPE_MASK(0, path, MAY_BE_NULL|MAY_BE_STRING, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_sys_dlg_color, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, pwboParent, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, title, MAY_BE_NULL|MAY_BE_STRING, "\"\"")
	ZEND_ARG_TYPE_MASK(0, color, MAY_BE_NULL|MAY_BE_LONG, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_sys_dlg_open, 0, 1, MAY_BE_STRING|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, pwboParent, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, title, MAY_BE_NULL|MAY_BE_STRING, "\"\"")
	ZEND_ARG_TYPE_MASK(0, filter, MAY_BE_NULL|MAY_BE_STRING, "\"\"")
	ZEND_ARG_TYPE_MASK(0, path, MAY_BE_NULL|MAY_BE_STRING, "\"\"")
	ZEND_ARG_TYPE_MASK(0, style, MAY_BE_NULL|MAY_BE_LONG, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_wb_sys_dlg_save, 0, 1, MAY_BE_NULL|MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO(0, pwboParent, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, title, MAY_BE_NULL|MAY_BE_STRING, "\"\"")
	ZEND_ARG_TYPE_MASK(0, filter, MAY_BE_NULL|MAY_BE_STRING, "\"\"")
	ZEND_ARG_TYPE_MASK(0, path, MAY_BE_NULL|MAY_BE_STRING, "\"\"")
	ZEND_ARG_TYPE_MASK(0, file, MAY_BE_NULL|MAY_BE_STRING, "\"\"")
	ZEND_ARG_TYPE_MASK(0, defext, MAY_BE_NULL|MAY_BE_STRING, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_wb_sys_dlg_font, 0, 0, IS_LONG, 0)
	ZEND_ARG_TYPE_MASK(0, pwbparent, MAY_BE_NULL|MAY_BE_LONG, "0")
	ZEND_ARG_TYPE_MASK(0, title, MAY_BE_NULL|MAY_BE_STRING, "\"\"")
	ZEND_ARG_TYPE_MASK(0, name, MAY_BE_NULL|MAY_BE_STRING, "\"\"")
	ZEND_ARG_TYPE_MASK(0, height, MAY_BE_NULL|MAY_BE_LONG, "\"\"")
	ZEND_ARG_TYPE_MASK(0, color, MAY_BE_NULL|MAY_BE_LONG, "\"\"")
	ZEND_ARG_TYPE_MASK(0, flags, MAY_BE_NULL|MAY_BE_LONG, "\"\"")
ZEND_END_ARG_INFO()