#ifndef RADIO_SK_SRC_CLIENT_MAIN_H_
#define RADIO_SK_SRC_CLIENT_MAIN_H_

#include "state.h"
#include "proxies_handler_socket.h"
#include "srp_receiver.h"
#include "sender.h"
#include "telnet_controller.h"
#include "stdout_metadata_proxy.h"
#include "../utils/addr.h"

namespace client {
class Main {
  State state_;
  ProxiesHandlerSocket sock_;
  proxy::StdoutMetadataProxy proxy_;
  types::addr_t proxy_addr_;
  srp::Receiver recv_;
  sender::DiscoverSender discover_sender_;
  sender::KeepaliveSender keepalive_sender_;
  telnet::Controller telnet_controller_;
  std::thread controller_worker_;
  std::thread keepalive_worker_;

 public:
  Main() = delete;
  Main(const std::string &proxy_host, const uint16_t proxy_port,
       const uint16_t telnet_port, const types::seconds_t timeout) :
      state_(timeout),
      proxy_(state_),
      proxy_addr_(addr::ip_port_to_addr(proxy_port, proxy_host)),
      recv_(proxy_, state_, sock_, proxy_addr_),
      discover_sender_(sock_, proxy_addr_),
      keepalive_sender_(sock_, state_),
      telnet_controller_(telnet_port, state_, discover_sender_),
      controller_worker_(std::ref(telnet_controller_)),
      keepalive_worker_(std::ref(keepalive_sender_)) {}

  void main() {
    controller_worker_.detach();
    keepalive_worker_.detach();
    recv_();
  }
};
}

#endif //RADIO_SK_SRC_CLIENT_MAIN_H_
