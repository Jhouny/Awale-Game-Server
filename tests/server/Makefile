CC = gcc
CFLAGS = -I/usr/local/include -g
LDFLAGS = -L/usr/local/lib -lcriterion

all: test_server

test_server: test_server.c
	$(CC) $(CFLAGS) -o test_server test_server.c $(LDFLAGS)

clean:
	rm -f test_server