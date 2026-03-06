# Scintilla Control for WinBinder (PHP Editor Focus): Phased Delivery Plan

## Goal
Add a native Scintilla-backed WinBinder control with practical PHP-editor capabilities, delivered incrementally to reduce risk and keep each step shippable.

## Phase 1 — Control Foundation (MVP)
**Outcome:** A Scintilla control can be created via `wb_create_control()` and used as a plain text editor.

### Scope
- Add `ScintillaEdit` to WinBinder control class enum and validation.
- Export PHP constant (`ScintillaEdit`) in module init.
- Add control creation branch in `wbCreateControl()` (`class = "Scintilla"`).
- Load `SciLexer.dll` during initialization with graceful failure handling.
- Add minimal sample that creates the control and sets text.

### Acceptance criteria
- `wb_create_control($win, ScintillaEdit, ...)` returns a valid control.
- Control renders and accepts typed text.
- Missing `SciLexer.dll` fails predictably (clear warning, no crash).

---

## Phase 2 — Essential Editor API Surface
**Outcome:** PHP userland can perform core editing operations without raw Win32 message plumbing.

### Scope
- Add wrappers + PHP bindings for core operations:
  - set/get text, append text, clear all
  - selection start/end, set selection, current position
  - go to line, get line count
  - read-only mode, undo/redo, cut/copy/paste
- Add style/config helpers:
  - tab width, tabs vs spaces, indentation guides
  - line number margin on/off

### Acceptance criteria
- A sample script can load/save text and move caret/selection.
- Basic editing commands work via dedicated WinBinder APIs.

---

## Phase 3 — PHP Editor Preset + Syntax Highlighting
**Outcome:** Scintilla behaves like a PHP-focused code editor out of the box.

### Scope
- Add a high-level preset helper (or documented setup routine):
  - UTF-8/code page defaults
  - PHP lexer selection
  - PHP keyword sets
  - default font/theme styles
  - folding and margin glyphs
  - brace matching and visual cues
- Add sample `php_editor.phpw` demonstrating preset usage.

### Acceptance criteria
- PHP syntax highlighting is functional and readable.
- Folding and line-number gutter are enabled.
- Sample opens as a usable PHP code editor.

---

## Phase 4 — Event Model for Interactive Editing
**Outcome:** WinBinder callback pipeline exposes useful Scintilla notifications for editor workflows.

### Scope
- Handle Scintilla notifications in `WM_NOTIFY` and map to WinBinder callback events.
- Prioritize:
  - `SCN_MODIFIED` (dirty tracking)
  - `SCN_UPDATEUI` (caret/selection updates)
  - `SCN_MARGINCLICK` (fold toggles)
  - optional `SCN_CHARADDED` (auto-indent/autocomplete triggers)
- Export event constants for PHP callback discrimination.

### Acceptance criteria
- Callback receives distinct event codes for modified/update/margin actions.
- A sample demonstrates dirty-state and fold-click handling.

---

## Phase 5 — PHP IDE Ergonomics (Nice-to-Have)
**Outcome:** Better coding experience for everyday PHP editing.

### Scope
- Autocomplete trigger strategy for identifiers, `$`, `->`, `::`.
- Simple call-tip support (where practical).
- Smart indentation for braces/new lines.
- Optional whitespace/line-ending visualization toggles.

### Acceptance criteria
- Autocomplete can be triggered with common PHP patterns.
- Indentation behavior is predictable for block edits.

---

## Cross-Phase Guardrails
- Keep each phase independently releasable.
- Prefer small, explicit PHP APIs over one opaque “send raw message” API.
- Add/update samples each phase to serve as executable documentation.
- Maintain graceful behavior when Scintilla runtime is unavailable.

## Suggested Delivery Order
1. Phase 1
2. Phase 2
3. Phase 3
4. Phase 4
5. Phase 5

This order ensures creation/stability first, then usability, then advanced editor ergonomics.
