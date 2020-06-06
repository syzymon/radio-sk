#ifndef RADIO_SK_SRC_PROXY_CLIENTS_HANDLER_SOCKET_H_
#define RADIO_SK_SRC_PROXY_CLIENTS_HANDLER_SOCKET_H_

#include <string>
#include <cstdint>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../utils/types.h"
#include "../utils/exceptions.h"
#include "../utils/udp.h"

static constexpr size_t BUF_SIZE = 65536;

class ClientsHandlerSocket : public udp::UDPClient<BUF_SIZE> {
  using super = udp::UDPClient<BUF_SIZE>;
  [[maybe_unused]] sockaddr_in local_address{};

  void init_mcast(const std::string &multi) {
    int optval = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval,
                   sizeof optval) < 0)
      safe_throw();

    ip_mreq ip_mreq{};
    ip_mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (inet_aton(multi.c_str(), &ip_mreq.imr_multiaddr) == 0) {
      safe_throw_custom(exceptions::InvalidInputException(
          "ERROR: inet_aton - invalid multicast address"
      ));
    }

    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &ip_mreq,
                   sizeof ip_mreq) < 0)
      safe_throw();
  }

  void set_socket_options(const std::optional<std::string> &multi) {
    if (multi)
      init_mcast(*multi);
  }

 public:
  ClientsHandlerSocket() = delete;

  explicit ClientsHandlerSocket(uint16_t server_port,
                       const std::optional<std::string> &multi = std::nullopt) :
      super() {
    set_socket_options(multi);
    local_address = bind_socket(sock, server_port);
  }
};

#endif //RADIO_SK_SRC_PROXY_CLIENTS_HANDLER_SOCKET_H_
