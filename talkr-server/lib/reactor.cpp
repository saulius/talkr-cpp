#include <reactor.h>

using namespace std;

Clients Reactor::connections;


void Reactor::create_socket() {
	int flag, res;
	int result = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // packet-format / two-way connection / transmission control protocol
	if (result != -1) {
		flag = fcntl(fd, F_GETFL, 0);
		if (flag == -1) {
			throw ServerException("[Reactor::create_socket] couldnt set the proper flag for the socket", true);
		}
		flag |= (O_NDELAY);
		res = fcntl(fd, F_SETFL, flag);
		if (res == -1) {
			throw ServerException("[Reactor::create_socket] couldnt make socket non-blocking", true);
		}
		fd = result;
		add_connection();
	} else {
		throw ServerException("[Reactor::create_socket] couldnt create the socket", true);
	}
}

int Reactor::bind(struct sockaddr * addr, size_t length) {
	int res;
	res = ::bind(fd, addr, length);
	if (res == -1) {
		throw ServerException("[Reactor::bind] couldnt bind the socket", true);
	} 
	return res;
}

int Reactor::listen(unsigned int n) {
	int res;
	accepting = true;
	res = ::listen(fd, n);
	if (res == -1) {
		throw ServerException("[Reactor::listen] couldnt start listening", true);
	}
	return res;
}

int Reactor::accept(struct sockaddr * addr, int * length_ptr) {
	int res;
	res = ::accept(fd, addr, (socklen_t*)length_ptr);
	if (res == -1) {
		throw ServerException("[Reactor::accept] couldnt accept connection", true);
	}
	return res;
}

int Reactor::send(const char * buffer, size_t size) {
	int result = ::send(fd, buffer, size, 0);

	if (result == size || result >= 0) {
		return result;
	} else {
		this->on_error(result);
		this->on_close();
		close();
		closed = true;
		return -1;
	}
}

int Reactor::recv(char * buffer, size_t size) {
	int result = ::recv(fd, buffer, size, 0);

	if (result > 0) {
		// data received
		return result;
	} else if (result == 0) {
		// client disconnected
		closed = true;
		this->on_close();
		return 0;
	} else {
		this->on_error(result);
		this->on_close();
		close();
		closed = true;
		return -1;
	} 
}

void Reactor::close(void) {
	this->closed = true;
	::close(fd);
}

void Reactor::set_fd(int fd) {
	this->fd = fd;
	connected = true;
	add_connection();
}


void Reactor::add_connection() {
	connections[fd] = this;
}

void Reactor::handle_read_event(void) {
	if (accepting) {
		if (!connected) {
			connected = true;
		}
		this->on_accept();
	} else if (!connected) {
		this->on_connect();
		connected = true;
		this->on_read();
	} else {
		this->on_read();
	}
}

void Reactor::handle_write_event(void) {
	if (!connected) {
		this->on_connect();
		connected = true;
	}
	this->on_write(); 
}

void Reactor::poll(struct timeval * timeout) {
	if (connections.size() > 0) {
		fd_set r,w;
		map<int, Reactor*>::iterator i;
		FD_ZERO (&r);
		FD_ZERO (&w);

		int num = 0;
		
		delete_closed_connections();
		if (!connections.size()) {
			return;
		}

		for (i = connections.begin(); i != connections.end(); i++) {
			int fd = (*i).first;
			if (((*i).second)->readable()) {
				FD_SET (fd, &r);
				num++;
			}
			if (((*i).second)->writable()) {
				FD_SET (fd, &w);
				num++;
			}
		}
	
		if (!num) {
			return;
		}
		int n = select(FD_SETSIZE, &r, &w, 0, timeout);
		for (i = connections.begin(); (i != connections.end() && n); ++i) {
			int fd = (*i).first;
			if (FD_ISSET (fd, &r)) {
				((*i).second)->handle_read_event();
				n--;
			}
			if (FD_ISSET (fd, &w)) {
				((*i).second)->handle_write_event();
				n--;
			}
		}
	}
}

void Reactor::loop(struct timeval * timeout) {
	while (connections.size()) {
		poll (timeout);
	}
}

void Reactor::delete_closed_connections() {
	list<int> clients;

	for (Clients::iterator i = connections.begin(); i != connections.end(); i++) {
		if (((*i).second)->closed) {
		 	clients.push_front((*i).first);
		}
	}
	for (list<int>::iterator h = clients.begin(); h != clients.end(); h++) {
		connections.erase(*h);
	}

}
