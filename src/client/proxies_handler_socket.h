#ifndef RADIO_SK_SRC_CLIENT_PROXIES_HANDLER_SOCKET_H_
#define RADIO_SK_SRC_CLIENT_PROXIES_HANDLER_SOCKET_H_

#include "../utils/udp.h"

static constexpr size_t BUF_SIZE = 65536;

class ProxiesHandlerSocket : public udp::UDPClient<BUF_SIZE> {
  using super = udp::UDPClient<BUF_SIZE>;
  static constexpr int TTL_VALUE = 4;

 public:
  ProxiesHandlerSocket() {
    init_bcast();
    int optval = TTL_VALUE;
    if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &optval, sizeof optval) < 0)
      throw exceptions::SocketException(errno);
  }
};

#endif //RADIO_SK_SRC_CLIENT_PROXIES_HANDLER_SOCKET_H_
