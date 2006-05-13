# Makefile

CC		= cc
RM		= rm


.SUFFIXES: .c .o
.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

OBJS =	gen.o

gen: $(OBJS)
	$(CC) $(OBJS) -ludis86 -o gen
test16: gen
	yasm -f bin -o test16.bin test16.asm
	echo "[bits 16]" > test16.out
	./gen -16 < test16.bin >> test16.out
	diff test16.asm test16.out | more
test32: gen
	yasm -f bin -o test32.bin test32.asm
	echo "[bits 32]" > test32.out
	./gen -32 < test32.bin >> test32.out
	diff test32.asm test32.out | more
test64: gen
	yasm -f bin -o test64.bin test64.asm
	echo "[bits 64]" > test64.out
	./gen -64 < test64.bin >> test64.out
	diff test64.asm test64.out | more
testjmp: gen
	yasm -f bin -o testjmp.bin testjmp.asm
	udcli -64 < testjmp.bin > testjmp.out
	cat testjmp.out | more
clean:
	$(RM) -f core ./*.o ./gen *~ *.bin *.out
