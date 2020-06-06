#ifndef RADIO_SK_SRC_CLIENT_SRP_RECEIVER_H_
#define RADIO_SK_SRC_CLIENT_SRP_RECEIVER_H_

#include <iostream>

#include "../utils/sik_radio_protocol.h"
#include "../utils/receiver.h"
#include "../utils/addr.h"
#include "stdout_metadata_proxy.h"
#include "proxies_handler_socket.h"

namespace srp {
class Receiver : public AbstractReceiver {
  ProxiesHandlerSocket &client_;
  const types::addr_t &proxy_addr_;
  State &state_;

  void dispatch_msg(const srp::Message &msg) {
    switch (msg.type()) {
      case srp::AUDIO:on_audio(msg.body());
        break;
      case srp::METADATA:on_metadata(msg.body());
        break;
    }
  }

  [[noreturn]] void listen() {
    while (true) {
      auto incoming_msg = client_.get_msg();
      auto req = srp::Message::decode(incoming_msg.first);
      auto current_client_addr = state_.get_current_client_addr();

//      std::cerr << "MSG: " << req.type() << "PROXY: " << (current_client_addr != std::nullopt) << '\n';

      if (req.type() == srp::IAM) {
        state_.add_proxy(incoming_msg.second, req.body());
      } else if (current_client_addr && addr::addr_equal(incoming_msg.second, *current_client_addr)) {
        state_.set_last_msg_time();
        dispatch_msg(req);
      }
      // TODO: do not send keepalive in this thread
      auto resp = srp::Message(srp::KEEPALIVE);
      client_.send_msg({resp.encode(), proxy_addr_});
    }
  }

 public:
  Receiver(proxy::StdoutMetadataProxy &proxy, State &state,
           ProxiesHandlerSocket &client, const types::addr_t &proxy_addr) :
      AbstractReceiver(proxy), client_(client), proxy_addr_(proxy_addr), state_(state) {}

  [[noreturn]] void operator()() override {
    listen();
  }
};
}

#endif //RADIO_SK_SRC_CLIENT_SRP_RECEIVER_H_
