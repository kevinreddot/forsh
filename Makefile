forsh: forsh.c config.h
	gcc -o forsh -static -O2 forsh.c
	strip forsh

clean:
	rm -f forsh
