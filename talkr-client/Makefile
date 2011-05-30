all: talkr
	
talkr: clientException.o reactor.o client.o
	g++ -g -Iinclude/ clientException.o reactor.o client.o talkr.cpp -o talkr
clientException.o: lib/clientException.cpp include/clientException.h
	g++ -g -Iinclude/ -c lib/clientException.cpp
reactor.o: lib/reactor.cpp include/reactor.h
	g++ -g -Iinclude/ -c lib/reactor.cpp
client.o: lib/client.cpp include/client.h
	g++ -g -Iinclude/ -c lib/client.cpp
clean:
	rm -rf *.o talkr
distclean:
	rm -rf *.o