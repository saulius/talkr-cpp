#include <client.h>

Messages Client::imsgs;
Messages Client::omsgs;
string Client::ip;
string Client::hostname;
string Client::nick;

void Client::initialize(char* host, char* port, char* name) {
	string data;
	quit = false;
	create_socket();
	data.append(name, strlen(name));
	nick = data;
	connect(host, port);
}

void Client::on_read(void) {
	char buffer[4096];
	vector<string> tokens;
	string sep = "\n";
	string data;
	int result = Reactor::recv(buffer, 4096);
	data.append(buffer, result);
	tokenize(data, tokens, sep);
	for (vector<string>::iterator it = tokens.begin(); it != tokens.end(); it++ ) {
		dispatch_message(*it);
	}
	return;
}

void Client::on_close(void) {
	quit = true;
}

void Client::on_connect(void) {
	omsgs.push("HANDSHAKE " + nick);
}

void Client::on_write(void) { 
	Reactor::send(omsgs.front().c_str(), omsgs.front().size());
	omsgs.pop();
	return;
}

void Client::handle_read_from_stdin(void) {
	string data;
	getline(cin, data);
	omsgs.push(data); 
}

bool Client::writable(void) {
	return (omsgs.size() > 0);
}

string Client::get_command(string msg) {
	return get_argument(msg, 0);
}

void Client::dispatch_message(string msg) {
	string command = get_command(msg);
	string argument;
	if (msg.length() > 512) { msg = msg.substr(0,512); }
	if (command == "NOTICE") {
		cout << "[!] " + get_full(msg) << endl;
	} else if ( command == "PMSG" ) {
		cout << "[>] <" + get_argument(msg, 1) + "> " + get_argument(msg, 2) << endl;
	} else if ( command == "SJOIN" ) {
		cout << "[J] #" + get_argument(msg, 1) << endl;
	} else if ( command == "S_PART" ) {
		cout << "[P] #" + get_argument(msg, 1) << endl;
	} else if ( command == "S_CMSG" ) {
	} else if ( command == "S_PMSG" ) {
	} else if ( command == "CMSG" ) {
		cout << "[>] #" + get_argument(msg, 1) + " <" + get_argument(msg, 2) + "> " + get_argument(msg, 3) << endl;
	} else if ( command == "E_PART" ) {
		cout << "[E] Taves nera tokiam kanale." << endl;
	} else if ( command == "E_PMSG" ) {
		cout << "[E] Nera tokio vartotojo." << endl;
	} else if ( command == "E_CMSG" ) {
		cout << "[E] Neesi tokiam kanale." << endl;
	} else {
		cout << "[E] " + msg << endl;
	}
}

void Client::tokenize(const string& str, token_vector& tokens, string& delimiters) {
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    string::size_type pos = str.find_first_of(delimiters, lastPos);
    while (string::npos != pos || string::npos != lastPos) {
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastPos);
    }
} 
 
string Client::get_argument(string msg, int position) {
	vector<string> tokens;
	string result;
	string sep = " ";
	tokenize(msg, tokens, sep);
	result = tokens.at(position);
	clean(result);
	return result;
}

string Client::get_full(string msg) {
	return msg.substr(msg.find(" "), msg.size());
}


void Client::clean(string &str) {
    string temp;
    for (unsigned int i = 0; i < str.length(); i++)
        if (str[i] != ' ' && str[i] != '\n' && str[i] != '\t' && str[i] != '\r' ) temp += str[i];
    str = temp;
}

