EXEC     = vgen.exe
CC       = gcc
CFLAGS   = -std=gnu11 -g -O3 -march=native -msse4.2 -funroll-loops -fomit-frame-pointer -Wall -Wextra -Wstrict-aliasing
LDFLAGS  = -lm
SRC      = $(wildcard *.c)
OBJ      = $(SRC:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJ) ed25519/libed25519.a hashset/libhashset.a crc16/libcrc16.a base32/libbase32.a
	$(CC) -o $@ $^ $(LDFLAGS)

hashset/libhashset.a:
	@make -C hashset

crc16/libcrc16.a:
	@make -C crc16

base32/libbase32.a:
	@make -C base32

ed25519/libed25519.a:
	@make -C ed25519

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
	$(CC) -MM $(CFLAGS) $*.c > $*.d

.PHONY: clean

clean:
	@rm -rf *.o *.d

cleanall:
	@make -C crc16 clean
	@make -C base32 clean
	@make -C hashset clean
	@make -C ed25519 clean
	@rm -rf *.o *.d $(EXEC)

prod:
	@strip vgen.exe

-include $(OBJ:.o=.d)