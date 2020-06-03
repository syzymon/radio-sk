#ifndef RADIO_SK_SRC_PROXY_CLIENTS_HANDLER_SOCKET_H_
#define RADIO_SK_SRC_PROXY_CLIENTS_HANDLER_SOCKET_H_

#include <string>
#include <cstdint>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "utils/types.h"
#include "utils/exceptions.h"

static constexpr size_t BUF_SIZE = 65536;

class ClientsHandlerSocket {
  int sock = -1;
  sockaddr_in local_address{};
  char buffer[BUF_SIZE]{};

  void safe_throw() const {
    close(sock);
    throw exceptions::SocketException(errno);
  }

  void safe_throw_custom(const std::exception &exc) const {
    close(sock);
    throw exc;
  }

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

 public:
  ClientsHandlerSocket() = delete;
  ClientsHandlerSocket(const ClientsHandlerSocket &other) = delete;

  explicit ClientsHandlerSocket(
      uint16_t server_port,
      const std::optional<std::string> &multi = std::nullopt
  ) {
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
      throw exceptions::SocketException(errno);

    if (multi)
      init_mcast(*multi);

    local_address = {
        .sin_family=AF_INET,
        .sin_port=htons(server_port),
        .sin_addr={
            .s_addr=htonl(INADDR_ANY)
        },
        .sin_zero{}
    };

    if (bind(sock, reinterpret_cast<const sockaddr *>(&local_address),
             sizeof local_address) < 0)
      safe_throw();
  }

  [[nodiscard]] types::NetworkMessage get_msg() {
    sockaddr_in client_address{};
    socklen_t rcva_len = sizeof client_address;
    ssize_t len = recvfrom(sock, buffer, sizeof(buffer),
                           0, reinterpret_cast<sockaddr *>(&client_address),
                           &rcva_len);
    if (len < 0)
      // TODO: safe throw here? refactor
      safe_throw();
    return {std::string(buffer, buffer + len), {client_address, rcva_len}};
  }

  void send_msg(const types::NetworkMessage &msg) const {
    if (sendto(sock, msg.first.c_str(), msg.first.length(), 0,
               reinterpret_cast<const sockaddr *>(&msg.second.first), msg.second.second) < 0)
      // TODO: refactor exception handling?
      safe_throw();
  }

  ~ClientsHandlerSocket() {
    if (sock >= 0) close(sock);
  }
};

#endif //RADIO_SK_SRC_PROXY_CLIENTS_HANDLER_SOCKET_H_
