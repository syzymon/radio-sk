#ifndef RADIO_SK_SRC_CLIENT_SENDER_H_
#define RADIO_SK_SRC_CLIENT_SENDER_H_

#include <thread>
#include "proxies_handler_socket.h"
#include "state.h"
#include "../utils/sik_radio_protocol.h"

namespace sender {
class DiscoverSender {
  const ProxiesHandlerSocket &socket_;
  const types::addr_t &proxy_addr_;
  const types::buffer_t discover_raw_;

 public:
  DiscoverSender() = delete;

  DiscoverSender(const ProxiesHandlerSocket &sock, const types::addr_t &proxy_addr)
      : socket_(sock), proxy_addr_(proxy_addr),
        discover_raw_(srp::Message(srp::DISCOVER).encode()) {}

  void discover_all() const {
    static const types::NetworkMessage discover_msg = {discover_raw_, proxy_addr_};
    socket_.send_msg(discover_msg);
  }

  void discover_single(const types::addr_t &proxy_unit_addr) const {
    socket_.send_msg({discover_raw_, proxy_unit_addr});
  }
};

class KeepaliveSender {
  const ProxiesHandlerSocket &socket_;
  State &state_;

 public:
  KeepaliveSender() = delete;

  KeepaliveSender(const ProxiesHandlerSocket &sock, State &state) : socket_(sock), state_(state) {}

  [[noreturn]] void operator()() {
    const types::buffer_t keepalive_raw = srp::Message(srp::KEEPALIVE).encode();
    using namespace std::chrono_literals;
    while (true) {
      auto addr = state_.get_current_client_addr();
      if (addr)
        socket_.send_msg({keepalive_raw, *addr});
      std::this_thread::sleep_for(3.5s);
    }
  }
};
}

#endif //RADIO_SK_SRC_CLIENT_SENDER_H_
