STATIC   = libbase32.a
CC       = gcc
CFLAGS   = -std=gnu11 -g -O3 -march=native -msse4.2 -funroll-loops -fomit-frame-pointer -Wall -Wextra -Wpedantic -Wstrict-aliasing
SRC      = $(wildcard *.c)
OBJ      = $(SRC:.c=.o)

all: $(STATIC)

$(STATIC): $(OBJ)
	ar rcs $@ $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
	$(CC) -MM $(CFLAGS) $*.c > $*.d

.PHONY: clean

clean:
	@rm -rf *.o *.a *.d

-include $(OBJ:.o=.d)
