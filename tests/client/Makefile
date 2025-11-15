CC=gcc
CFLAGS=-I/usr/local/include -g
LDFLAGS=-lcriterion

all: test_client

test_client: test_client.c
	$(CC) $(CFLAGS) -o test_client test_client.c $(LDFLAGS)

clean:
	rm -f test_client