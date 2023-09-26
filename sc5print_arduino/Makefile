default: test

test:
	@gcc -D TEST sc5print.c sc5print_test.c -o sc5print
	@./sc5print test_in.sc5

dump:
	@hd test_out.pcl

print:
	@nc 192.168.50.50:9100 < test_out.pcl
