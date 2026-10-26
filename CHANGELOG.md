# Changelog

## Unreleased (0.2)

### New

- A help screen shows the keys you can press in the current mode. It opens by itself at the `:` prompt and after `d`, `c`, `Z` and `r`. BREAK opens and closes it at any time, and `:set help=off` turns it off.
- Large files. EviL uses all the free RAM for text, and the first status message shows how much space there is.
- A file that does not fit is refused with "File too big", and the text you have stays as it is. If the file named on the command line is too big, EviL starts with no file name, so `:w` cannot write empty text over it.
- The cursor keys work in insert and replace mode.
- Messages from `:` commands stay on screen.
- A NextGuide manual, `evil.gde`.

### Fonts

- New fonts: Envious, Envious Bold, Envious Italic, Envious Serif, Envious Serif Bold, Magic 5, Magic 5 Bold, Plotter, Plotter Bold and Plotter Condensed.
- `:set font` on its own opens a font picker. The screen changes as you move through the list. ENTER keeps the font, BREAK goes back to the old one.
- `font = /path/file.ch8` loads your own 768-byte CH8 font.
- `60s` is now called `Computing60s`. The old name still works.
- New short names: `ROM` (Sinclair), `QE` (Cinema), `OCRA` and `OCRB`.
- Font names ignore case.

### Fixes

- On quit, EviL clears the screen to black on white with a white border.
- Settings from `EviL.RC` load before EviL sets up its palettes.
- A failed save shows "Failed to save file" with the error number, and beeps.
- "No filename set" and "Document not saved (use ! to confirm)" now beep.
- The error beep is shorter and flashes the border magenta.
- Corrections to `docs/settings.md`.
- `:w` and `:wq` actually save the text now, instead of random bytes of my own code...
- `:w NAME` and `:wq NAME` save the text as NAME, as in vi. Before, they only set the file name. When NAME is a different file that exists, EviL asks for `:w! NAME`.
- `ZZ` on text with no file name shows "No filename set". Before, the message was cleared at once.
- `d2w` deletes 2 words, not 12. Counts multiply, so `2d3w` deletes 6.
- `dw` and `cw` delete the whole word when the cursor is at its start.
- `dd` on an empty line and `d$` at the end of a line no longer delete the next line too.
- `r` then ENTER puts a line break in place of the character.
- Changes made with `r` count as changes, so `ZZ` saves them.
- Colour settings use all 9 bits.
- `cursor_flash` works for every blink, not only the first.
- `EviL.RC` files with Windows line ends, no newline at the end, very long lines or settings with no value now load correctly.
- `:set` on its own, an empty `:` command and very long `:` commands no longer corrupt memory.
- An unknown font name shows an error only when `errors` is on.
- Error windows replace the help window instead of drawing on top of it.
- The border no longer stays red after a save.
- Old characters no longer stay on the bottom rows of the screen.
- Control keys no longer type control characters in insert mode.

### Build

- Builds with zsdcc in z88dk's Docker image, `z88dk/z88dk:latest`.
- The build fails if BANK_COMMAND grows past 8K.
- `make zip` builds EviL and makes `RELEASE-EviL/EviL-<VERSION>.zip` for SPUN, with `EviL.DOT`, `evil.rc`, `settings.md` and `evil.gde`.
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
