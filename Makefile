default: gcc

SRCS := $(wildcard *.c)

z88dk:
	@zcc -O3 +msx -DTARGET_MSX -subtype=msxdos2 -pragma-output:USING_amalloc -o"D:\Tools\msx\BlueMsx\DirAsDisk\sc5print.com" -lm $(SRCS)

gcc:
	@gcc $(SRCS) -o"bin\sc5print"