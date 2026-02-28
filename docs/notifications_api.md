# Notifications API (`wb_notify`)

`wb_notify(array $options)` provides lightweight notifications with a system-first strategy and a built-in fallback.

## Options

- `parent` (`int`, optional): WinBinder window object pointer. Needed for Windows tray balloon integration attempts.
- `title` (`string`, optional): Notification title. Defaults to `WinBinder notification`.
- `body` (`string`, optional): Notification message text.
- `icon` (`string|int`, optional): `info` (default), `warning`, `error`, `none`, or a raw `NIIF_*` value.
- `duration` (`int`, optional): Milliseconds (default `5000`).
- `onClick` (`string`, optional): Reserved for future clickable notification actions (currently reported as unsupported).

## Return payload

`wb_notify` returns an array:

- `delivered` (`bool`): always `true` when the call completes.
- `backend` (`string`): `windows_toast` when toast integration succeeded, else `in_app_banner`.
- `system_supported` (`bool`): whether Windows tray balloon delivery was used.
- `click_callback_supported` (`bool`): currently `false`; clickable actions are explicitly unavailable on both backends.

## Compatibility behavior

1. **Preferred path:** attempts a native Windows toast via a hidden PowerShell call to `Windows.UI.Notifications` APIs.
2. **Fallback path:** when platform integration is unavailable, uses a non-blocking in-app fallback (window flash + system sound) instead of modal dialogs.
3. **Action callback:** callback wiring is intentionally explicit via `click_callback_supported=false` until a true clickable banner surface is added.

## Example

See: `docs/manual/examples/notifications_background_task.php`
