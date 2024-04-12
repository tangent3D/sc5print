default: msx gcc

msx:
	@zcc +msx -subtype=msxdos2 -pragma-output:USING_amalloc -o"D:\Dropbox\Projects\MSX\DirAsDisk UNAPITCPIP\sc5pcl.com" -lm sc5pcl.c

gcc:
	@gcc sc5pcl.c -o sc5pcl.exe
