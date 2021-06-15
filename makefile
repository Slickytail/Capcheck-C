CC=gcc

capcheck : capcheck.c
	$(CC) capcheck.c -O3 -Bsymbolic -fno-semantic-interposition -Wall -o capcheck

profile : capcheck.c
	$(CC) capcheck.c -o capcheck-prof -pg -Og
	./capcheck-prof -d 8 -s 19
	gprof capcheck-prof gmon.out

clean :
	rm -f capcheck capcheck-prof gmon.out
