STATIC   = libed25519.a
CC       = gcc
AS       = as
CFLAGS   = -std=gnu11 -g -O3 -funroll-loops -fomit-frame-pointer -msse4.2 -Wall -Wextra -Wpedantic -Wstrict-aliasing
SRC      = $(wildcard *.c)
ASM      = $(wildcard *.s)
OBJ      = $(SRC:.c=.o)
OBJ     += $(ASM:.s=.o)

all: $(STATIC)

$(STATIC): $(OBJ)
	ar rcs $@ $^

%.o: %.s
	$(AS) -o $@ $<

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
	$(CC) -MM $(CFLAGS) $*.c > $*.d

.PHONY: clean

clean:
	@rm -rf *.o *.a *.d

-include $(OBJ:.o=.d)
