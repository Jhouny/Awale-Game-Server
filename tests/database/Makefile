CC=gcc
CFLAGS=-I/usr/local/include -g
LDFLAGS=-L/usr/local/lib -lcriterion

all: test_database

test_database: test_database.o
	$(CC) -o test_database test_database.o $(LDFLAGS)

test_database.o: test_database.c
	$(CC) $(CFLAGS) -c test_database.c

clean:
	rm -f test_database test_database.o

run: test_database
	./test_database