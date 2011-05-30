#include <clientException.h>

ClientException::ClientException(const string &message, bool debug) throw() : msg(message) {
  if (debug) {
    msg.append(": ");
    msg.append(strerror(errno));
  }
}

ClientException::~ClientException() throw() {
}

const char *ClientException::what() const throw() {
  return msg.c_str();
}

