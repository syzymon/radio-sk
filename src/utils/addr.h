#ifndef RADIO_SK_SRC_UTILS_ADDR_H_
#define RADIO_SK_SRC_UTILS_ADDR_H_

#include <arpa/inet.h>
#include "types.h"

namespace addr {
types::addr_t ip_port_to_addr(const uint16_t port, const std::optional<std::string> &ip = std::nullopt) {
  sockaddr_in sa{};
  sa.sin_family = AF_INET;
  if (ip)
    inet_pton(AF_INET, ip->c_str(), &(sa.sin_addr));
  else
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
  sa.sin_port = htons(port);
  return {sa, sizeof sa};
}

bool addr_equal(const types::addr_t &a, const types::addr_t &b) {
  return a.first.sin_addr.s_addr == b.first.sin_addr.s_addr && a.first.sin_port == b.first.sin_port;
}
}

#endif //RADIO_SK_SRC_UTILS_ADDR_H_
