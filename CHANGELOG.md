# Changelog

## Unreleased (0.2)

### New

- A help screen lists the keys of the current mode, with the keys in red. It opens by itself at the `:` prompt and after `d`, `c`, `Z` and `r`, and closes when EviL goes back to normal mode or into insert mode. BREAK opens and closes it in any mode. The `help` setting turns it off.
- In insert and replace mode, the cursor keys move the cursor.

### Fonts

- New bundled fonts: Envious, Envious Bold, Envious Italic, Envious Serif, Envious Serif Bold, Magic 5, Magic 5 Bold, Plotter, Plotter Bold and Plotter Condensed.
- The 60s font is now named `Computing60s`. `60s` still works as an alias.
- New font aliases: `ROM` for the Sinclair font, `QE` for Cinema (the font QE uses), `OCRA` and `OCRB`.
- Font names in settings are case insensitive.
- `font` with no value is accepted. It does nothing yet.

### Fixes

- On quit, EviL clears the screen to black on white with a white border.
- Settings from `EviL.RC` load before EviL sets up its palettes.
- A failed save shows "Failed to save file" with the error number, and beeps.
- "No filename set" and "Document not saved (use ! to confirm)" now beep.
- The error beep is shorter and flashes the border magenta.
- Corrections to `docs/settings.md`.
- `:w` and `:wq` actually save the text now, instead of random bytes of my own code...
- The border no longer stays red after a save.
- Colour settings set the 9th colour bit.
- `cursor_flash` sets the time for every blink of the cursor, not only for the first.
- `EviL.RC` reads files with CRLF line ends, a last line with no newline, and settings with no value. A line longer than 511 characters is cut short; before, it overwrote memory.
- `:set` with no setting, an empty `:` command and a command longer than 125 characters no longer write to memory outside their buffers.
- EviL no longer allocates two 16K banks that it does not use.
- A count typed after `d` or `c` sets the count for that command: `d2w` deletes 2 words. Before, `d2w` deleted 12. Counts before and after multiply, so `2d3w` deletes 6 words.
- `dw` and `cw` delete up to the start of the next word. Before, at the start of a word they deleted one character.
- `dd` on an empty line and `d$` at the end of a line no longer also delete the next line.
- `r` then ENTER replaces the character with a line break. Before, ENTER did nothing and the DOWN key put the line break.
- A change made with `r` is saved by `ZZ`, and `:q` asks for `!`. Before, `r` did not mark the text as changed.
- The bottom text row is cleared when the text ends above it.
- In insert mode, control keys other than the cursor keys no longer insert control characters.

### Build

- Builds with zsdcc in z88dk's Docker image, `z88dk/z88dk:latest`.
- `make zip` builds EviL and makes `RELEASE-EviL/EviL-<VERSION>.zip` for SPUN, with `EviL.DOT`, `evil.rc` and `settings.md`.
- The development workflow moved to Linux, and `make emulate` runs MAME.

## v0.1-pr2 (2020-11-20)

### New

- Colour settings for the text window, cursor, status bar and error bar, in `EviL.RC` and with `:set`.
- The border takes the paper colour.
- EviL sets its palette directly and restores the palette when it quits.
- The status bar takes one row less, so the text window has one row more.
- An example `evil.rc`.

### Fixes

- Key repeat no longer stutters at the start.
- The "Unknown Settings" window draws correctly.
- The error message for a setting that must be more than 0 is formatted correctly.
- The splash screen matches the rest of the app.

## v0.1-pr1 (2020-11-19)

First preview release. EviL is a fork of QE with more NextZXOS features.

- Code split into memory banks by function.
- Settings at runtime with `:set`.
- Settings loaded from `/SYS/EviL.RC` at start.
- Key repeat with a configurable first delay and repeat rate.
- Configurable cursor flash speed.
- Bundled fonts: Sinclair, Cinema, Courier, Magnetic, Minecraft, OCR-A, OCR-B and 60s.
- Error and setting windows in the Stale Pixels UI style.
- `On`/`Off` accepted as well as `true`/`false`.
- No crash when EviL cannot create its temporary file.
