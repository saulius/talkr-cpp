all: talkr
	
talkr: serverException.o reactor.o server.o
	g++ -g -Iinclude/ serverException.o reactor.o server.o talkr.cpp -o talkr
serverException.o: lib/serverException.cpp include/serverException.h
	g++ -g -Iinclude/ -c lib/serverException.cpp
reactor.o: lib/reactor.cpp include/reactor.h
	g++ -g -Iinclude/ -c lib/reactor.cpp
server.o: lib/server.cpp include/server.h
	g++ -g -Iinclude/ -c lib/server.cpp
clean:
	rm -rf *.o talkr
distclean:
	rm -rf *.o