all: server client test

server: server.o
	g++ server.o -o server -pthread
	
client: client.o
	g++ -o client client.o

server.o: server.cpp 
	g++ -c server.cpp

client.o: client.cpp 
	g++ -c client.cpp

test: tests.o 
	g++ tests.o -o test -pthread

test.o: tests.o
	g++ -c tests.cpp 

clear:
	find . -name "*.o" | xargs rm -f server client server client

.PHONY: clean all

clean:
	rm -rf *.o server client test

