#ifndef _CLIENTEXCEPTION_H
#define _CLIENTEXCEPTION_H

#include <string>
#include <errno.h>
#include <exception>

using namespace std;

class ClientException : public exception {
  
  public:
    ClientException(const string &message, bool debug = false) throw();
    ~ClientException() throw();
    const char *what() const throw();

  private:
    string msg;
};

#endif

