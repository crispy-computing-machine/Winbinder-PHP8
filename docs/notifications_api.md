# Notifications API (`wb_notify`)

`wb_notify(array $options)` provides lightweight notifications with a system-first strategy and a built-in fallback.

## Options

- `parent` (`int`, optional): WinBinder window object pointer. Needed for Windows tray balloon integration attempts.
- `title` (`string`, optional): Notification title. Defaults to `WinBinder notification`.
- `body` (`string`, optional): Notification message text.
- `icon` (`string|int`, optional): `info` (default), `warning`, `error`, `none`, or a raw `NIIF_*` value.
- `duration` (`int`, optional): Milliseconds (default `5000`).
- `onClick` (`string`, optional): PHP callback function name used by fallback notifications.

## Return payload

`wb_notify` returns an array:

- `delivered` (`bool`): always `true` when the call completes.
- `backend` (`string`): `windows_tray_balloon` when tray integration succeeded, else `in_app_banner`.
- `system_supported` (`bool`): whether Windows tray balloon delivery was used.
- `click_callback_supported` (`bool`): `false` for tray balloons in this implementation, `true` for fallback mode.

## Compatibility behavior

1. **Preferred path:** attempts Windows notification-area balloon via `Shell_NotifyIcon(NIM_MODIFY)`.
2. **Fallback path:** when platform integration is unavailable, uses an in-app timed banner fallback (`wb_quiet_message_box` behavior).
3. **Action callback:** `onClick` executes only on fallback mode when the user confirms the banner. This makes callback support explicit and deterministic across platforms.

## Example

See: `docs/manual/examples/notifications_background_task.php`
