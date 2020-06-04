#ifndef RADIO_SK_SRC_CLIENT_SRP_RECEIVER_H_
#define RADIO_SK_SRC_CLIENT_SRP_RECEIVER_H_

#include <iostream>

#include "utils/sik_radio_protocol.h"
#include "utils/receiver.h"
#include "proxies_handler_socket.h"

namespace srp {
class Receiver : public AbstractReceiver {
  ProxiesHandlerSocket &client;
  const types::addr_t &proxy_addr;

  void say_hello() {
    auto msg = srp::Message(srp::DISCOVER);
    client.send_msg({msg.encode(), proxy_addr});
  }

  void dispatch_msg(const srp::Message& msg, types::addr_t to) {
    switch (msg.type()) {
      case srp::AUDIO:
        on_audio(msg.body());
        break;
      case srp::METADATA:
        on_metadata(msg.body());
        break;
    }
  }

  [[noreturn]] void listen() {
    while (true) {
      auto incoming_msg = client.get_msg();
      auto req = srp::Message::decode(incoming_msg.first);

      if(req.type() == srp::IAM) {
        std::cerr << "I AM";
        // TODO:
      } else {
        dispatch_msg(req, incoming_msg.second);
      }
      auto resp = srp::Message(srp::KEEPALIVE);
      client.send_msg({resp.encode(), proxy_addr});
    }
  }

 public:
  Receiver(proxy::StdoutProxy &proxy,
           ProxiesHandlerSocket &client, const types::addr_t &proxy_addr) :
      AbstractReceiver(proxy), client(client), proxy_addr(proxy_addr) {}

  [[noreturn]] void stream_content() override {
    say_hello();
    listen();
  }
};
}

#endif //RADIO_SK_SRC_CLIENT_SRP_RECEIVER_H_
