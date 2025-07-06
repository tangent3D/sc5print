default: msx gcc

msx:
#	@zcc -O3 +msx -DTARGET_MSX -subtype=msxdos2 -L"third-party\asmlib.lib" -pragma-output:USING_amalloc -o"bin\sc5print.com" -lm sc5print.c
	@zcc -O3 +msx -DTARGET_MSX -subtype=msxdos2 -L"third-party\asmlib.lib" -pragma-output:USING_amalloc -o"C:\Users\Tangent\Desktop\DirAsDisk\sc5print.com" -lm sc5print.c

gcc:
	@gcc sc5print.c -o"bin\sc5print.exe"
