cmake_minimum_required(VERSION 3.10)
project(EVIL C)


include_directories(
        /u/ZX/z88dk/include/_DEVELOPMENT/sccz80
)

add_executable(EVIL
# MAIN RAM
        src/evil

        src/common/file
        src/common/memory

        src/liblayer3/clear
        src/liblayer3/cleareol
        src/liblayer3/getc
        src/liblayer3/goto
        src/liblayer3/newline
        src/liblayer3/putc
        src/liblayer3/puts
        src/liblayer3/revoff
        src/liblayer3/revon
        src/liblayer3/textmode

# 46
        src/BANK_command/error
        src/BANK_command/file_name_set
        src/BANK_command/file_save
        src/BANK_command/parse
        src/BANK_command/status
# 47
        src/BANK_system/startup
        src/BANK_spui/error.c src/BANK_spui/error.h src/BANK_settings/load.c src/BANK_settings/load.h src/BANK_fonts/library.c src/BANK_fonts/library.h src/BANK_fonts/set.c src/BANK_fonts/set.h src/BANK_settings/apply.c src/BANK_settings/apply.h src/BANK_fonts/get.c src/BANK_fonts/get.h src/BANK_spui/font_invalid.c src/BANK_spui/font_invalid.h src/BANK_spui/window.c src/BANK_spui/window.h src/BANK_spui/dialog.c src/BANK_spui/dialog.h src/BANK_spui/button.c src/BANK_spui/button.h src/BANK_spui/font_not_found.c src/BANK_spui/font_not_found.h src/BANK_spui/triangle.c src/BANK_spui/triangle.h src/BANK_spui/line.c src/BANK_spui/line.h)
