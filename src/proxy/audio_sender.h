#ifndef RADIO_SK_SRC_PROXY_AUDIO_SENDER_H_
#define RADIO_SK_SRC_PROXY_AUDIO_SENDER_H_

#include "utils/sik_radio_protocol.h"
#include "clients_handler_socket.h"
#include "clients_pool.h"

class ContentSender {
  ClientsHandlerSocket &sock;
  pool::ClientsPool &pool;

  void send_single(const types::addr_t &addr, const srp::Message &msg) const {
    sock.send_msg({msg.encode(), addr});
  }
 public:
  ContentSender() = delete;

  ContentSender(ClientsHandlerSocket &sock, pool::ClientsPool &pool) : sock(sock), pool(pool) {}

  void send_to_clients(const srp::Message &msg) const {
    auto sendable_clients = pool.sendable_clients();
    std::for_each(sendable_clients.begin(), sendable_clients.end(),
        [this, &msg](const types::addr_t & client_addr) {send_single(client_addr, msg);});
  }
};

#endif //RADIO_SK_SRC_PROXY_AUDIO_SENDER_H_
