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
        )
