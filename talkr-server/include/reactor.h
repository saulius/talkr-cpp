// vim: ts=4:noai:sw=4
/**
 *  Event based server
 *
 */

#ifndef REACTOR_H
#define REACTOR_H

#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/time.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <list>
#include <queue>

#include <serverException.h>

using namespace std;

class Reactor;

typedef map<int, Reactor*, less<int> > Clients;

class Reactor {

	private:
		bool accepting, connected, closed;  						/* current state */
	protected:
		static Clients connections;
		int fd;

	public:

	Reactor () : fd(false), accepting(false), connected(false), closed(false) { }


	void create_socket();
	void set_fd(int fd);
	int bind(struct sockaddr * addr, size_t length);
	int listen(unsigned int n);
	int accept(struct sockaddr * addr, int * length_ptr);
	int send(const char * buffer, size_t size);
	int recv(char * buffer, size_t size);
	void close(void);

	void add_connection();

	static void poll(struct timeval * timeout = 0);
	static void loop(struct timeval * timeout = 0);

	int get_fd () { return fd; }
	static void delete_closed_connections();

	virtual bool readable (void) { return (connected || accepting); }
	virtual bool writable (void) { return (!connected); }

	void handle_read_event(void);
	void handle_write_event(void);

	virtual void on_connect(void) { cout << fd << "connect not implemented" << endl; }
	virtual void on_read(void) { cout << fd << "read not implemented" << endl; }
	virtual void on_write(void) { cout << fd << "write not implemented" << endl; }
	virtual void on_close(void) { cout << fd << "close not implemented" << endl; }
	virtual void on_accept(void) { cout << fd << "accept not implemented" << endl; }
	virtual void on_error(int error) { cout << fd << "error occured" << error << " error: " << strerror(errno) << endl; }
};

#endif // REACTOR_H

