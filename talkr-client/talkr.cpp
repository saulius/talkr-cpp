// vim: noai:ts=4:sw=4

#include <cstdio>
#include <string>
#include <iostream>
#include <unistd.h>
#include <client.h>

using namespace std;

void print_help();

int main(int argc, char* argv[]) {
	Client* talkr;
	int param;
	char* port;
	char* host;
	char* nick;

	if (argc < 4) {
		print_help();
		exit(0);
	} else {
		host = argv[1];
		port = argv[2];
		nick = argv[3];
	}

	talkr = new Client();
	talkr->initialize(port, host, nick);
	talkr->loop(0);
	return 0;
}



void print_help() {
	cout << "talkr chat client. Usage:" << endl;
	cout << "talkr <server ip address or hostname> <port> <nick>" << endl;
}


