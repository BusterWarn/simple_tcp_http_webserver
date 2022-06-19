CC = gcc
CFLAGS = -std=gnu11 -g -Wall -Wextra -Werror -Wmissing-declarations -Wmissing-prototypes -Werror-implicit-function-declaration -Wreturn-type -Wparentheses -Wunused -Wold-style-definition -Wundef -Wshadow -Wstrict-prototypes -Wswitch-default -Wunreachable-code

all:			webserver

webserver:		webserver.o str.o
	$(CC) webserver.o str.o -o webserver

webserver.o:	webserver.c webserver.h
	$(CC) $(CFLAGS) -c webserver.c

str.o:			str.c str.h
	$(CC) $(CFLAGS) -c str.c

run:			webserver
	./webserver

valgrind1: 		webserver
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes ./webserver 6001 /home/buster/Documents/UMU/kurser/dod/ou1

valgrind2: 		webserver
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes ./webserver 6002 /home/buster/Documents/UMU/kurser/dod/ou1

valgrind3: 		webserver
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes ./webserver 6003 /home/buster/Documents/UMU/kurser/dod/ou1

clean:
	rm -f webserver *.o core
