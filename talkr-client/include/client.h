#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <list>
#include <queue>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

#include <reactor.h>

typedef queue<string> Messages;
typedef map<int,int> MessageCount;
typedef vector<string> token_vector;

class Client : public Reactor {
	private:
		string buffer;
		static Messages imsgs;
		static Messages omsgs;
		static string ip;
		static string hostname;
		static string nick;

	public:

		Client() {}

		bool writable(void);
		void initialize(char* host, char* port, char* nick);
		void on_read(void);
		void on_write(void);
		void on_connect(void);
		void on_close(void);
		void handle_read_from_stdin(void);

		void c_handshake(string msg);
		void c_pmsg(string msg);
		void c_cmsg(string msg);
		void c_sversion(string msg);
		void c_join(string msg);
		void c_part(string msg);

		void dispatch_message(string msg);
		string get_command(string msg);
		string get_argument(string msg, int position);
		string get_full(string msg);
		void clean(string &str);
		void tokenize(const string& str, token_vector& tokens, string& delimiters);
};

#endif // CLIENT_H

