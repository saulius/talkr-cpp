// vim: noai:ts=4:sw=4

#include <cstdio>
#include <string>
#include <iostream>
#include <unistd.h>
#include <server.h>

using namespace std;

void print_help();

int main(int argc, char* argv[]) {
	Server* talkr;
	int param;
	 
	if (argc > 1) {
		while ((param = getopt (argc, argv, "hp:")) != -1)
			switch (param) {
				case 'h':
						print_help();
						break;
					case 'p':
						talkr = new Server();
						talkr->initialize(optarg);
						Reactor::loop(0);
						break;
					case '?':
						if (optopt == 'p') fprintf (stderr, "Please specify value for -%c.\n", optopt);
						else if (isprint (optopt)) fprintf (stderr, "Unknown option `-%c'.\n", optopt);
						else fprintf (stderr, "Unknown value for option `\\x%x'.\n", optopt);
						break;
					default:
						print_help();
						break;
					}
	} else {
		print_help();
	}
  return 0;
}



void print_help() {
	cout << "talkr chat server. Run params:" << endl;
	cout << "-p <port numeris>" << endl;
	cout << "-h - show help" << endl;
}


