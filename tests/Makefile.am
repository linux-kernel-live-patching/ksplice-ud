#
# Automake definitiosn for ud/tests
#
INCLUDES       = -I../libudis86/
bin_PROGRAMS   = gen ovrrun
gen_SOURCES    = gen.c
gen_LDADD      = ../libudis86/libudis86.la
ovrrun_SOURCES = bufovrrun.c
ovrrun_LDADD   = ../libudis86/libudis86.la

tests: test16 test32 test64 testjmp bufovrrun randraw

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
	../udcli/udcli -64 < testjmp.bin > testjmp.out
	diff testjmp.ref testjmp.out

testobscure: gen
	yasm -f bin -o testobscure.bin testobscure.asm
	./gen -32 < testobscure.bin > testobscure.out
	diff testobscure.ref testobscure.out

bufovrrun: ovrrun
	./ovrrun

randraw: randtest.raw
	./gen -16 < randtest.raw > randtest16.out
	diff randtest16.out randtest16.ref
	./gen -32 < randtest.raw > randtest32.out
	diff randtest32.out randtest32.ref
	./gen -64 < randtest.raw > randtest64.out
	diff randtest64.out randtest64.ref

clean-local:
	$(RM) -f *.bin *.out
