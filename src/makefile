
CFLAGS = -g -Wall -Wextra -Iutil -Lutil

all: main  test

main: main.c H.c testH.c 
	$(CC) $(CFLAGS) -Wc++-compat -o main main.c H.c -lbitarr -lpthread
	$(CC) $(CFLAGS) -Wc++-compat -o testH testH.c H.c


prof: main.c H.c testH.c
	$(CC) $(CFLAGS) -o main main.c H.c -lbitarr -lpthread -O3
	$(CC) $(CFLAGS) -o testH testH.c H.c -O3
	

clean:
	rm -rf main testH H *.o *.dSYM *.greg

.PHONY: all clean test
