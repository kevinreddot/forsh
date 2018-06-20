forsh: forsh.c config.h
	gcc -o forsh -Wpedantic -Wall -Wextra -fstack-protector-strong -Wformat -Werror=format-security -D_FORTIFY_SOURCE=2 -fPIE -pie -O2 forsh.c
	strip forsh

clean:
	rm -f forsh

install: forsh
	mkdir -p $(DESTDIR)/usr/local/bin
	install -o 0 -g 0 -m 0755 forsh $(DESTDIR)/usr/local/bin/cello
