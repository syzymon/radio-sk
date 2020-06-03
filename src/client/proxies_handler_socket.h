#ifndef RADIO_SK_SRC_CLIENT_PROXIES_HANDLER_SOCKET_H_
#define RADIO_SK_SRC_CLIENT_PROXIES_HANDLER_SOCKET_H_

#include "utils/udp.h"

static constexpr size_t BUF_SIZE = 65536;

class ProxiesHandlerSocket : public udp::UDPClient<BUF_SIZE> {
  using super = udp::UDPClient<BUF_SIZE>;

 public:
  ProxiesHandlerSocket() {
    init_bcast();
  }
};

#endif //RADIO_SK_SRC_CLIENT_PROXIES_HANDLER_SOCKET_H_
