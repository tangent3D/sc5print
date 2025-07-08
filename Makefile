default: z88dk gcc

z88dk:
	@zcc -O3 +msx -DTARGET_MSX -subtype=msxdos2 -pragma-output:USING_amalloc -o"D:\Tools\msx\BlueMsx\DirAsDisk\sc5print.com" -lm main.c tcp.c

gcc:
	@gcc main.c tcp.c -o"bin\sc5print"