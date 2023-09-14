default: win

win:
	@gcc sc5print.c -o sc5print
	@./sc5print in.sc5

dump:
	@hd out.pcl

msx:
	@zcc +msx -subtype=msxdos -osc5print.com -lm sc5print.c

print:
	@nc 192.168.50.50:9100 < out.pcl
