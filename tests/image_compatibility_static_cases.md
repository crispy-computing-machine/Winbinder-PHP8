# Image compatibility static regression cases

These manual/static cases validate `wb_load_image()` behavior and bitmap-only API guards.

## Raster decode coverage

- `wb_load_image("fixtures/sample.png")` should return a non-null bitmap handle (`IsBitmap == TRUE`).
- `wb_load_image("fixtures/sample.jpg")` should return a non-null bitmap handle.
- `wb_load_image("fixtures/sample.jpeg")` should return a non-null bitmap handle.
- `wb_load_image("fixtures/sample.gif")` should return a non-null bitmap handle.
- `wb_load_image("fixtures/sample.bmp")` should return a non-null bitmap handle.
- `wb_load_image("fixtures/sample.wbmp")` should return a non-null bitmap handle when decoder support exists.
- Optional/decoder-dependent: `.webp`, `.xbm`, `.avif` should return bitmap handles when decoder support exists; otherwise return NULL with a decode error.

## Icon/cursor compatibility (unchanged behavior)

- `wb_load_image("fixtures/sample.ico")` should return an icon handle (`IsIcon == TRUE`).
- `wb_load_image("fixtures/sample.icl", 0, 0)` should continue icon library extraction behavior.
- `wb_load_image("fixtures/sample.dll", 0, 1)` and `.exe` should continue extraction behavior.
- `wb_load_image("fixtures/sample.cur")` and `.ani` should continue cursor load behavior.

## Unsupported/invalid format failures

- `wb_load_image("fixtures/not_an_image.txt")` should return NULL and show an unrecognized format error.
- `wb_load_image("fixtures/broken.png")` should return NULL and show unsupported/invalid raster format error.
- Mixed-case extension should still work: `wb_load_image("fixtures/SAMPLE.PnG")`.
- Substring false positives should not match: `wb_load_image("fixtures/image.bmp.backup")` should fail as unrecognized.

## Bitmap-only API guards

Given `hIco = wb_load_image("fixtures/sample.ico")`:

- `wb_save_image(hIco, "out.bmp")` should fail and return `FALSE`.
- `wb_get_image_data(hIco)` should fail and return `NULL`.
- `wb_rotate_image(hIco, 90)` should fail and return `NULL`.
- `wb_resize_image(hIco, 32, 32)` should fail and return `NULL`.

Given `hBmp = wb_load_image("fixtures/sample.png")`:

- `wb_save_image(hBmp, "out.bmp")` should succeed.
- `wb_get_image_data(hBmp)` should return a non-empty string.
- `wb_rotate_image(hBmp, 90)` should return a non-null bitmap handle.
- `wb_resize_image(hBmp, 32, 32)` should return a non-null bitmap handle.
