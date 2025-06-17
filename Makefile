default: msx gcc

msx:
	@zcc +msx -subtype=msxdos2 -L"third-party\asmlib.lib" -pragma-output:USING_amalloc -o"bin\sc5pcl.com" -lm sc5pcl.c

gcc:
	@gcc sc5pcl.c -o"bin\sc5pcl.exe"
