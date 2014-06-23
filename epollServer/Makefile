CC=g++
INCLUDE=.

all: client server
client: client.o
	$(CC) -o client client.o -lpthread
client.o: client.cpp
	$(CC) -I$(INCLUDE) -c client.cpp
server: main.o epollserver.o ProcessPool.o
	$(CC) -o server main.o epollserver.o ProcessPool.o
main.o: main.cpp epollserver.cpp ProcessPool.cpp
	$(CC) -I$(INCLUDE) -c main.cpp epollserver.cpp ProcessPool.cpp
clean:
	rm *.o
