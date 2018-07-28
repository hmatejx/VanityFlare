STATIC   = libed25519.a
CC       = gcc
AS       = as
CFLAGS   = -std=gnu11 -g -O3 -march=native -msse4.2 -funroll-loops -fomit-frame-pointer -Wall -Wextra -Wpedantic -Wstrict-aliasing
SRC      = $(wildcard *.c)
ASM      = $(wildcard *.s)
ASM2     = $(wildcard *.S)
OBJ      = $(SRC:.c=.o)
OBJ     += $(ASM:.s=.o)
OBJ     += $(ASM2:.S=.o)

all: $(STATIC)

$(STATIC): $(OBJ)
	ar rcs $@ $^

%.o: %.S
	$(CC) -o $@ -c $<

%.o: %.s
	$(AS) -o $@ $<

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
	$(CC) -MM $(CFLAGS) $*.c > $*.d

.PHONY: clean

clean:
	@rm -rf *.o *.a *.d

-include $(OBJ:.o=.d)
