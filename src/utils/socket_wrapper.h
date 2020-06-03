//
// Created by syzymon on 01.06.2020.
//

#ifndef RADIO_SK_SRC_UTILS_SOCKET_WRAPPER_H_
#define RADIO_SK_SRC_UTILS_SOCKET_WRAPPER_H_

#include <netinet/in.h>
#include <unistd.h>
#include <netinet/in.h>

#include "exceptions.h"

namespace network {
class AbstractSocketWrapper {
 protected:
  int sock = -1;

  virtual void safe_throw() const {
    close(sock);
    throw exceptions::SocketException(errno);
  }

  virtual void safe_throw_custom(const std::exception &exc) const {
    close(sock);
    throw exc;
  }

  explicit AbstractSocketWrapper(int sock) : sock(sock) {}
 public:
  virtual ~AbstractSocketWrapper() = default;
};
}

#endif //RADIO_SK_SRC_UTILS_SOCKET_WRAPPER_H_
