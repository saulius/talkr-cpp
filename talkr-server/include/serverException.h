#ifndef _SERVEREXCEPTION_H
#define _SERVEREXCEPTION_H

#include <string>
#include <errno.h>
#include <exception>

using namespace std;

class ServerException : public exception {

  public:
    ServerException(const string &message, bool debug = false) throw();
    ~ServerException() throw();
    const char *what() const throw();

  private:
    string msg;
};

#endif

