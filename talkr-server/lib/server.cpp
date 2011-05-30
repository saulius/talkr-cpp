#include <server.h>

Messages Server::msgs;
MessageCount Server::msgc;
Mapping Server::cmap;
Channels Server::chanmap;

void Server::initialize(char* port) {
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	create_socket();
	bind((struct sockaddr *) &addr, sizeof(addr));
	listen(5);
}

void Server::on_accept(void) {
	struct sockaddr addr;
	int addr_len = sizeof(sockaddr);
	int fd = accept(&addr, &addr_len);
	Server* s = new Server;
	s->set_fd(fd);

	msgc.insert(make_pair(fd,0));
	msgs.insert(make_pair(fd,new MessageQueue));
}

void Server::on_close(void) {
	msgc.erase(fd);
	msgs.erase(fd);
	close();
}

void Server::on_read(void) {
	char buffer[512];
	string data;
	int result = Reactor::recv(buffer, 512);
	data.append(buffer, result);
	dispatch_message(data);
	return;
}

void Server::on_write(void) {
	// descriptors 0 - stdin, 1 - stdout, 2 - stderr, 3 - listening server
	if (this->fd > 3 && msgc.find(this->fd)->second > 0) {
		string msg = msgs.find(this->fd)->second->front();
		Reactor::send(msg.c_str(), msg.size());
		msgs.find(this->fd)->second->pop();
		msgc.find(this->fd)->second--;
	}
	return;
}

bool Server::writable(void) {
	return (msgc.find(this->fd)->second > 0);
}

void Server::say(string msg, int fd) {
	msgs.find(fd)->second->push(msg + "\n"); 
	msgc.find(fd)->second++;
}

void Server::say(string msg) {
	say(msg, this->fd);
}

void Server::c_handshake(string msg) {
	string argument = get_argument(msg, 1);
	string line, data;
	ifstream motd("motd");
	if (motd.fail()) {
		throw new ServerException("[Server::c_handshake] couldnt read the motd", true);
	}
	clean(argument);
	if (cmap.find(argument) != cmap.end()) {
		srand(time(NULL));
		argument = "guest" + inttostring(rand());
		say("NOTICE user with such nickname is already connect so yours was changed to " + argument);
	}
	this->nick = argument;
	cmap.insert(make_pair(argument, this->fd));
	say("NOTICE Hi, this is talkr server!");
	if (motd.is_open()) {
		while (! motd.eof() ) {
			getline (motd,line);
			say("NOTICE " + line);
		}
		motd.close();
	}
}

void Server::c_pmsg(string msg) {
	string argument = get_argument(msg, 1);
	if (cmap.find(argument) != cmap.end()) {
		say("S_PMSG " + argument);
		say("PMSG " + nick + " " + get_argument(msg, 2), get_fd_by_nick(argument));
	} else {
		say("E_PMSG " + argument);
	}
}

void Server::c_join(string msg) {
	string argument = get_argument(msg, 1);
	bool found = false;
	if (chanmap.find(argument) == chanmap.end()) {
		chanmap.insert(make_pair(argument, new CList()));
	}
	for (vector<int>::iterator it = chanmap.find(argument)->second->begin(); it != chanmap.find(argument)->second->end(); it++) {
		if (*it == this->fd) {
			found = true;
		} else {
			say("CMSG " + argument + " JOINS: " + nick + " " + get_argument(msg, 2), *it);
		}
	}
	if (!found) {
		chanmap.find(argument)->second->push_back(this->fd);
	}
	channels.push_back(argument);
	say("SJOIN "+argument);	
}

void Server::c_part(string msg) {
	string argument = get_argument(msg, 1);
	vector<int>::iterator it_d;
	vector<string>::iterator it_del;
	bool found = false;
	for (vector<string>::iterator i = channels.begin(); i != channels.end(); i++) {
		if (*i == argument) {
			found = true;
			it_del = i;
		}
	}
	if (!found) {
		say("E_PART "+argument);
	} else {
		channels.erase(it_del);
		for(vector<int>::iterator i = chanmap.find(argument)->second->begin(); i != chanmap.find(argument)->second->end(); i++) {
			if ((*i) == this->fd) {
				it_d = i;
			} else {
				say("CMSG " + argument + " PARTS: " + nick + " " + get_argument(msg, 2), *i);
			}
		}
		chanmap.find(argument)->second->erase(it_d);
		if (chanmap.find(argument)->second->size() == 0) {
			chanmap.erase(chanmap.find(argument));
		}
		say("S_PART "+argument);
	}
}

void Server::c_quit(string msg) {
	string argument = get_argument(msg, 1);
	Channels::iterator chan;
	vector<int>::iterator del_fd;
	bool del;
	for (vector<string>::iterator i = channels.begin(); i != channels.end(); i++) {
		for (map<string, CList*>::iterator it = chanmap.begin(); it != chanmap.end(); it++ ) {
			if (it->first == *i) {
				for(vector<int>::iterator j = it->second->begin(); j != it->second->end(); j++) {
					if (*j == this->fd) {
						del_fd = j;
					} else {
						say("CMSG " + *i + " QUITS: " + nick + " " + argument, *j);
					}
				}
			}
		}		
	}
	if (del) {
		chanmap.find(argument)->second->erase(del_fd);
	}
	on_close();
}

void Server::c_cmsg(string msg) {
	string argument = get_argument(msg, 1);
	for (vector<string>::iterator i = channels.begin(); i != channels.end(); i++) {
		if (*i == argument) {
			for (map<string, CList*>::iterator it = chanmap.begin(); it != chanmap.end(); it++ ) {
				if (it->first == argument) {
					say("S_CMSG " + argument);
					for(vector<int>::iterator j = it->second->begin(); j != it->second->end(); j++) {
						say("CMSG " + argument + " " + nick + " " + get_argument(msg, 2), *j);
					}
					return;
				}
			}
		}
	}
	say("E_CMSG " + argument);
	return;
}

string Server::get_command(string msg) {
	return get_argument(msg, 0);
}

void Server::dispatch_message(string msg) {
	string command; 
	string argument;
	
	if (msg.size() < 1) {
		on_close();
		return;
	}
	
	command = get_command(msg);
	
	if (command != "HANDSHAKE" && this->nick.empty()) {
		say(string("You dont have a nickname, no other actions can be performed. Please use HANDSHAKE command"));
		return;
	}

	if (command == "HANDSHAKE") {
		c_handshake(msg);
	}

	else if (command == "PMSG") {
		c_pmsg(msg);
	}

	else if (command == "SVERSION") {
		say("talkr server");
	}

	else if (command == "JOIN") {
		c_join(msg);
	}

	else if (command == "PART") {
		c_part(msg);
	}

	else if (command == "CMSG") {
		c_cmsg(msg);
	}
	
	else if (command == "QUIT") {
		c_quit(msg);
	}
	else {
		say("UNRECOGNIZED COMMAND");
	} 
}

int Server::get_fd_by_nick(string nick) {
	return cmap.find(nick)->second;
}

void Server::tokenize(const string& str, token_vector& tokens, string& delimiters) {
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    string::size_type pos = str.find_first_of(delimiters, lastPos);
    while (string::npos != pos || string::npos != lastPos) {
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastPos);
    }
}

string Server::get_argument(string msg, int position) {
	vector<string> tokens;
	string result;
	string sep = " ";
	tokenize(msg, tokens, sep);
	if (tokens.size() > position) {
		result = tokens.at(position);
		clean(result);
		return result;
	} else {
		return "no arg!";
	} 
}

void Server::clean(string &str) {
    string temp;
    for (unsigned int i = 0; i < str.length(); i++)
        if (str[i] != ' ' && str[i] != '\n' && str[i] != '\t' && str[i] != '\r' ) temp += str[i];
    str = temp;
}

string Server::inttostring(int n) {
	std::string s;
	std::stringstream out;
	out << n;
	return out.str();
}

