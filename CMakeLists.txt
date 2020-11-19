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
# 44
        src/BANK_settings/apply
        src/BANK_settings/ini_common
        src/BANK_settings/ini_get_one
        src/BANK_settings/ini_get_line
        src/BANK_settings/ini_set_one
        src/BANK_settings/load
        src/BANK_settings/trim_whitespace
# 46
        src/BANK_command/error
        src/BANK_command/file_name_set
        src/BANK_command/file_save
        src/BANK_command/parse
        src/BANK_command/status
# 47
        src/BANK_system/startup
        src/BANK_settings/load
        src/BANK_fonts/library
        src/BANK_fonts/set
        src/BANK_settings/apply
        src/BANK_fonts/get

# 47
        src/BANK_spui/error
        src/BANK_spui/font_invalid
        src/BANK_spui/window
        src/BANK_spui/dialog
        src/BANK_spui/button
        src/BANK_spui/font_not_found
        src/BANK_spui/triangle
        src/BANK_spui/line
        src/BANK_spui/not_zero
        )
