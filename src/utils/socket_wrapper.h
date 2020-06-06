#ifndef RADIO_SK_SRC_UTILS_SOCKET_WRAPPER_H_
#define RADIO_SK_SRC_UTILS_SOCKET_WRAPPER_H_

#include <iostream>
#include <netinet/in.h>
#include <unistd.h>

#include "exceptions.h"

namespace network {
class AbstractSocketWrapper {
 protected:
  int sock = -1;

  void safe_throw() const {
    close(sock);
    throw exceptions::SocketException(errno);
  }

  void safe_throw_custom(const std::exception &exc) const {
    close(sock);
    throw exc;
  }

  explicit AbstractSocketWrapper(int sock) : sock(sock) {}
 public:
  virtual ~AbstractSocketWrapper() {
    if (sock >= 0) close(sock);
  }
};
}

#endif //RADIO_SK_SRC_UTILS_SOCKET_WRAPPER_H_
