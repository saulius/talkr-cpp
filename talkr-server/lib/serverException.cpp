#include <serverException.h>

ServerException::ServerException(const string &message, bool debug) throw() : msg(message) {
  if (debug) {
    msg.append(": ");
    msg.append(strerror(errno));
  }
}

ServerException::~ServerException() throw() {
}

const char *ServerException::what() const throw() {
  return msg.c_str();
}

