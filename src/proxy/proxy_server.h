#ifndef RADIO_SK_SRC_PROXY_PROXY_SERVER_H_
#define RADIO_SK_SRC_PROXY_PROXY_SERVER_H_

#include "utils/sik_radio_protocol.h"
#include "clients_handler_socket.h"
#include "clients_pool.h"

class ProxyServer {
  ClientsHandlerSocket &sock;
  clients_pool::ClientsPool &pool;

 public:
  ProxyServer() = delete;

  explicit ProxyServer(ClientsHandlerSocket &sock, clients_pool::ClientsPool &pool) :
      sock(sock), pool(pool) {}

  [[noreturn]] void operator()() {
    while (true) {
      auto incoming_msg = sock.get_msg();
      auto request = srp::Message::decode(incoming_msg.first);
      switch (request.type()) {
        case srp::DISCOVER: {
          auto response = srp::Message(srp::IAM, "TODO");
          // Respond to the client with IAM.
          sock.send_msg({response.encode(), incoming_msg.second});
          // TODO: start sending only after first discover to unit address?
          pool.add_client(incoming_msg.second);
        }
          break;
        case srp::KEEPALIVE: {
          // TODO: update last date in the pool
          pool.update_client_time(incoming_msg.second);
        }
          break;
        default: break; // TODO: handle not recognized messages?
      }
    }
  }
};

#endif //RADIO_SK_SRC_PROXY_PROXY_SERVER_H_
