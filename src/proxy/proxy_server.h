#ifndef RADIO_SK_SRC_PROXY_PROXY_SERVER_H_
#define RADIO_SK_SRC_PROXY_PROXY_SERVER_H_

#include "../utils/sik_radio_protocol.h"
#include "clients_handler_socket.h"
#include "clients_pool.h"

class ProxyServer {
  ClientsHandlerSocket &sock;
  pool::ClientsPool &pool;
  const std::string &current_stream_name;
  const types::buffer_t &last_metadata;

 public:
  ProxyServer() = delete;

  explicit ProxyServer(ClientsHandlerSocket &sock, pool::ClientsPool &pool, const std::string &current_stream_name,
                       const types::buffer_t &last_metadata) :
      sock(sock), pool(pool), current_stream_name(current_stream_name), last_metadata(last_metadata) {}

  [[noreturn]] void operator()() {
    while (true) {
      auto incoming_msg = sock.get_msg();
      auto request = srp::Message::decode(incoming_msg.first);
      switch (request.type()) {
        case srp::DISCOVER: {
          auto response = srp::Message(srp::IAM, current_stream_name);
          sock.send_msg({response.encode(), incoming_msg.second});
          pool.add_client(incoming_msg.second);

          if (!last_metadata.empty()) {
            auto meta_response = srp::Message(srp::METADATA, last_metadata);
            sock.send_msg({meta_response.encode(), incoming_msg.second});
          }
        }
          break;
        case srp::KEEPALIVE: {
          pool.update_client_time(incoming_msg.second);
        }
          break;
        default: break;
      }
    }
  }
};

#endif //RADIO_SK_SRC_PROXY_PROXY_SERVER_H_
