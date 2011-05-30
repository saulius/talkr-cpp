// vim: ts=4:noai:sw=4

#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <list>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <vector>
#include <ctime>

#include <reactor.h>

typedef queue<string> MessageQueue;
typedef map<int,MessageQueue*> Messages;
typedef map<int,int> MessageCount;
typedef map<string, int> Mapping;
typedef vector<string> token_vector;
typedef vector<int> CList;
typedef map<string, CList*> Channels;

class Server : public Reactor {
	private:
		string buffer;
		/* messages for the clients */
		static Messages msgs;
		static MessageCount msgc;
		/* nick -> fd map */
		static Mapping cmap;
		static Channels chanmap;
		vector<string> channels;
		string ip;
		string hostname;
		string nick;

	public:

		Server() {}

		bool writable(void);
		void initialize(char* port);
		void say(string msg, int fd);
		void say(string msg);

		void on_accept(void);
		void on_read(void);
		void on_write(void);
		void on_close(void);

		void c_handshake(string msg);
		void c_pmsg(string msg);
		void c_cmsg(string msg);
		void c_sversion(string msg);
		void c_join(string msg);
		void c_part(string msg);
		void c_quit(string msg);

		int get_fd_by_nick(string msg); 
		void dispatch_message(string msg);
		string get_command(string msg);
		string get_argument(string msg, int position);
		void clean(string &str);
		void tokenize(const string& str, token_vector& tokens, string& delimiters);
		string inttostring(int n);
};

#endif // SERVER_H

