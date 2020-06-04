#ifndef RADIO_SK_SRC_CLIENT_MAIN_H_
#define RADIO_SK_SRC_CLIENT_MAIN_H_

#include "proxies_handler_socket.h"
#include "srp_receiver.h"
#include "utils/proxy.h"
#include "utils/addr.h"

namespace client {
class Main {
  ProxiesHandlerSocket sock_;
  proxy::StdoutProxy proxy_;
  types::addr_t proxy_addr_; // TODO: not needed here, only for mock?
  srp::Receiver recv_;

 public:
  Main() = delete;
  Main(const std::string &proxy_host, const uint16_t proxy_port,
       const uint16_t client_port, const types::seconds_t timeout) :
      proxy_addr_(addr::ip_port_to_addr(proxy_host, proxy_port)),
      recv_(proxy_, sock_, proxy_addr_) {}

  [[noreturn]] void main() {
    recv_.stream_content();
  }
};
}

#endif //RADIO_SK_SRC_CLIENT_MAIN_H_
