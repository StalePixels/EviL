NAME = EVIL
BUILD_DIR ?= ./BUILD-$(NAME)
TMP_DIR ?= ./TMP
SRC_DIRS ?= ./src
DOCS_DIR ?= ./docs
FILES_DIR ?= ./dist
RELEASE_DIR ?= ./RELEASE-$(NAME)
INSTALL_BASE ?= /Volumes/devnext
INSTALL_DIR ?= $(INSTALL_BASE)/$(NAME)

Z88DK_CRT = 1

#DEBUGFLAGS := --list --c-code-in-asm
DEBUGFLAGS =

CC=zcc
CCFLAGS=+zxn -vn -O3 -startup=$(Z88DK_CRT) -clib=new -preserve -pragma-include:zpragma.inc
CZFLAGS=-Cz="--clean --fullsize --main-fence 0xC000"
LDFLAGS=-m -lm
INCFLAGS=

SH := /bin/sh
ECHO := @/bin/echo -n
CD := cd
RSYNC := rsync -parv
MKDIR := mkdir -p
RM := rm -rfv
LSH := ls -larth
CP := @cp -rv
MV := @mv -v

all: dirs deps dotn

clean:
	$(RM) $(BUILD_DIR)
	$(RM) $(TMP_DIR)
	$(RM) $(NAME).map
	$(RM) $(NAME)_*.bin
	$(RM) cmake-build-debug
	$(RM) RELEASE/*
	$(RM) NAME

NAME:
	$(ECHO) $(NAME) > NAME

deps: NAME

dirs:
	$(MKDIR) $(BUILD_DIR)
	$(MKDIR) $(TMP_DIR)


bank_47:
	$(CC) $(CCFLAGS) -c $(DEBUG) $(INCFLAGS) $(BUILDFLAGS) \
		--codesegBANK_47 --constsegBANK_47 \
		--datasegBANK_47 --bsssegBANK_47   \
		evil_banked.c -o$(TMP_DIR)/evil_banked.o

banks: dirs deps bank_47

assemble: banks
	$(CC) $(CCFLAGS) $(LDFLAGS) @$(NAME).lst -o$(NAME) -create-app \
		-subtype=dotn $(CZFLAGS)
	$(MV) $(NAME) $(BUILD_DIR)/$(NAME)

dotn: banks assemble

install-dir:
	$(MKDIR) $(INSTALL_DIR)

update:
	$(CP) $(BUILD_DIR)/$(NAME) $(INSTALL_BASE)/dot/$(NAME)

install: install-dir update

deploy: all install

q: dotn
	$(CP) $(BUILD_DIR)/$(NAME) $(INSTALL_BASE)/dot/$(NAME)

qq: assemble update
