default:
	@gcc sc5print.c -o sc5print
	@./sc5print in.sc5
	@hd out.bin

msx:
	zcc +msx -subtype=msxdos -osc5print.com -lm sc5print.c
