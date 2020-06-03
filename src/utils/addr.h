#ifndef RADIO_SK_SRC_UTILS_ADDR_H_
#define RADIO_SK_SRC_UTILS_ADDR_H_

#include <arpa/inet.h>
#include "types.h"

namespace addr {
types::addr_t ip_port_to_addr(const std::string &ip, const uint16_t port) {
  sockaddr_in sa;
  sa.sin_family = AF_INET;
  inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr));
  sa.sin_port = htons(port);
  return {sa, sizeof sa};
}
}

#endif //RADIO_SK_SRC_UTILS_ADDR_H_
