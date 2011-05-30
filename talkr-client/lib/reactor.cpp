#include <reactor.h>

using namespace std;

void Reactor::create_socket() {
	int flag, res;
	int result = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); 
	if (result != -1) {
		flag = fcntl(fd, F_GETFL, 0);
		if (flag == -1) {
			throw ClientException("[Reactor::create_socket] couldnt set the socket flag", true);
		}
		flag |= (O_NDELAY);
		res = fcntl(fd, F_SETFL, flag);
		if (res == -1) {
			throw ClientException("[Reactor::create_socket] couldnt make the socket non blocking", true);
		}
		fd = result;
	} else {
		throw ClientException("[Reactor::create_socket] couldnt create the socket", true);
	}
}

int Reactor::bind(struct sockaddr * addr, size_t length) {
	int res;
	res = ::bind(fd, addr, length);
	if (res == -1) {
		throw ClientException("[Reactor::bind] couldnt bind on the socket", true);
	}
	return res;
}

int Reactor::listen(unsigned int n) {
	int res;
	accepting = true;
	res = ::listen(fd, n);
	if (res == -1) {
		throw ClientException("[Reactor::listen] couldnt start listening on socket", true);
	}
	return res;
}

int Reactor::accept(struct sockaddr * addr, int * length_ptr) {
	int res;
	res = ::accept(fd, addr, (socklen_t*)length_ptr);
	if (res == -1) {
		throw ClientException("[Reactor::accept] couldnt accept connection", true);
	}
	return res;
}

int Reactor::connect(char *port, char *address) {
	struct sockaddr_in addr;
	struct hostent *hostEntity = gethostbyname(address);
	if (hostEntity == 0) {
		throw new ClientException("[Reactor::connect] cannot connect to the host");
	}
	int arg = 1;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));
	memcpy(&addr.sin_addr, hostEntity->h_addr_list[0], hostEntity->h_length);
	memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
	setsockopt(this->fd, SOL_SOCKET,SO_REUSEADDR,&arg,sizeof(int));
	int result = ::connect (this->fd, (const struct sockaddr*)&addr, sizeof(addr));

	if (result == 0) {
		connected = 1;
		this->on_connect();
		return 0;
	} else {
		return -1;
	}
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
		return result;
	} else if (result == 0) {
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
	int res;
	res = ::close(fd);
	if (res != -1) {
		closed = true;
	} else {
		throw ClientException("[Reactor::close] couldnt close the socket", true);
	} 
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
	fd_set temp;
	FD_ZERO(&client);
	FD_SET(this->fd, &client);
	FD_SET(STDIN_FILENO, &client);
	
	if (this->writable()) {
		handle_write_event();
	}
	while (true) {
		if (quit) {
			return;
		}
		temp = client;
		int n = ::select(FD_SETSIZE, &temp, NULL, NULL, timeout);
		for(int f=FD_SETSIZE; f >= 0; f--) {
			if (FD_ISSET(f,&temp)) {
				if (f == this->fd) {
					handle_read_event();
				} else if (f == STDIN_FILENO) {
					handle_read_from_stdin();
					if (this->writable()) {
						handle_write_event();
					}
				} else {
					if (this->writable()) {
						handle_write_event();
					}
				} 
			}
		}
	}
}

void Reactor::loop(struct timeval * timeout) {
	poll(timeout);
}
