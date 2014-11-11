
CFLAGS = -g -Wall -Wextra -Iutil -Lutil

all: main  test

main: main.c H.c testH.c 
	$(CC) $(CFLAGS) -Wc++-compat -o main main.c H.c -lbitarr -lpthread
	$(CC) $(CFLAGS) -Wc++-compat -o testH testH.c H.c
	

clean:
	rm -rf main testH H *.o *.dSYM *.greg

.PHONY: all clean test
