// vim: ts=4:noai:sw=4

#ifndef REACTOR_H
#define REACTOR_H

#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/time.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <list>
#include <queue>

#include <clientException.h>

using namespace std;

class Reactor;

class Reactor {

	private:
		bool accepting, connected, closed;
	protected:
		int fd;
		fd_set client;
		bool quit;

	public:

	Reactor () : fd(false), accepting(false), connected(false), closed(false) { }

	void create_socket();
	void set_fd(int fd);
	int bind (struct sockaddr * addr, size_t length);
	int connect (char *port, char *address);
	int listen (unsigned int n);
	int accept (struct sockaddr * addr, int * length_ptr);
	int send (const char * buffer, size_t size);
	int recv (char * buffer, size_t size);
	void close (void);
	
	void add_connection();

	void poll(struct timeval * timeout = 0);
	void loop(struct timeval * timeout = 0);

	int get_fd () { return fd; }

	virtual bool readable (void) { return (connected || accepting); }
	virtual bool writable (void) { return (!connected); }

	void handle_read_event(void);
	void handle_write_event(void);

	  virtual void on_connect(void) { cout << fd << ":unhandled connect" << endl; }
	  virtual void on_read(void) {  cout << fd << ":unhandled read" << endl; }
	  virtual void handle_read_from_stdin(void) { cout << fd << ":unhandled read" << endl; }
	  virtual void handle_write_to_stdout(void) { cout << fd << ":unhandled write" << endl; }
	  virtual void on_write(void) { cout << fd << ":unhandled write" << endl; }
	  virtual void on_close(void) { cout << fd << ":unhandled close" << endl; }
	  virtual void on_accept(void) { cout << fd << ":unhandled accept" << endl; }
	  virtual void on_error(int error) { cout << fd << ":unhandled error:" << error << " error: " << strerror(errno) << endl; }   
};

#endif // REACTOR_H
