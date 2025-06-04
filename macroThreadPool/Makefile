.SUFFIXES: .md .3

CC		= cc
CFLAGS		= -Wall -pedantic -O2 
LDFLAGS		= -lpthread
PREFIX		= /usr/local
OBJFILES	= example.o
TARGET		= mtpExample

MANCC		= lowdown
MANFLAGS	= -s
MANDIR		= $(PREFIX)/share/man
MANTARGETS	= macroThreadPool.3

ifeq ($(OS),Windows_NT)
TARGET = aout.exe
else
UNAME_S	:= $(shell uname -s)
ifeq ($(UNAME_S),Linux)
else
ifeq ($(UNAME_S),Darwin)
else
$(error unknown system, "$(UNAME_S)")
endif # Darwin
endif # Linux
endif # Windows

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

debug: CFLAGS = -Wall -pg -Wextra -Wpedantic -ggdb -Og -DDEBUG
debug: CFLAGS += -fsanitize=address -fsanitize=leak
debug: CFLAGS += -fsanitize=undefined
debug: CFLAGS += -Wdouble-promotion -Wformat -Wformat-overflow
debug: CFLAGS += -Wnull-dereference -Winfinite-recursion
debug: CFLAGS += -Wstrict-overflow -Wno-unused-function -Wconversion
debug: all

rebuild: clean
rebuild: all

manpage: $(MANTARGETS)


.md.3:
	$(MANCC) $(MANFLAGS) -o $@ -tman $<

clean:
	rm -f $(OBJFILES) $(TARGET) $(MANTARGETS)

help:
	@echo "Makefile options:"
	@echo "make         : builds the example program"
	@echo "make debug   : builds with address sanitizer enabled"
	@echo "make rebuild : calls clean before rebuilding example program"
	@echo "make clean   : removes object files, executable, and manpage"
	@echo "make manpage : Build the man page, requires lowdown(1)"
	@echo "make help    : Prints this message"

.PHONY: install uninstall clean rebuild manpage help
