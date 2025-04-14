.SUFFIXES: .md .3

CC		= cc
CFLAGS		= -Wall -pedantic -O2 -ansi
LDFLAGS		= 
PREFIX		= /usr/local
OBJFILES	= 
TARGET		= 

MANCC		= lowdown
MANFLAGS	= -s
MANDIR		= $(PREFIX)/share/man
MANTARGETS	= manCev.3

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

.PHONY: install uninstall clean rebuild manpage
