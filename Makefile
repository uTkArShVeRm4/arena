# === Configuration ===
NAME    = arena
PREFIX  ?= /usr/local

# === Derived ===
SRC     = $(NAME).c
HDR     = $(NAME).h
OBJ     = $(NAME).o

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    LIBEXT = dylib
else
    LIBEXT = so
endif

TARGET = lib$(NAME).$(LIBEXT)
CFLAGS  = -Wall -fPIC
LDFLAGS = -shared

# === Rules ===
.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ): $(SRC) $(HDR)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

install: $(TARGET)
	install -d $(PREFIX)/lib
	install -d $(PREFIX)/include
	install -m 644 $(TARGET) $(PREFIX)/lib/
	install -m 644 $(HDR) $(PREFIX)/include/

uninstall:
	rm -f $(PREFIX)/lib/$(TARGET)
	rm -f $(PREFIX)/include/$(HDR)

clean:
	rm -f $(OBJ) $(TARGET)

