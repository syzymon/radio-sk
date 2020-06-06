#ifndef RADIO_SK_SRC_UTILS_UDP_H_
#define RADIO_SK_SRC_UTILS_UDP_H_

#include "types.h"
#include "socket_wrapper.h"

namespace udp {
template<int BUFFER_SIZE>
class UDPClient : public network::AbstractSocketWrapper {
  char buffer[BUFFER_SIZE]{};

  static int create_socket() {
    int sock;
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
      throw exceptions::SocketException(errno);
    return sock;
  }

 protected:
  void init_bcast() {
    int optval = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &optval, sizeof optval) < 0)
      safe_throw();
  }

  static sockaddr_in bind_socket(int sock, uint16_t port) {
    sockaddr_in addr = {
        .sin_family=AF_INET,
        .sin_port=htons(port),
        .sin_addr={
            .s_addr=htonl(INADDR_ANY)
        },
        .sin_zero{}
    };

    if (bind(sock, reinterpret_cast<const sockaddr *>(&addr),
             sizeof addr) < 0) {
      close(sock);
      throw exceptions::SocketException(errno);
    }
    return addr;
  }

 public:
  [[nodiscard]] types::NetworkMessage get_msg() {
    sockaddr_in client_address{};
    socklen_t rcva_len = sizeof client_address;
    ssize_t len = recvfrom(sock, buffer, sizeof(buffer),
                           0, reinterpret_cast<sockaddr *>(&client_address),
                           &rcva_len);
    if (len < 0)
      safe_throw();
    return {std::string(buffer, buffer + len), {client_address, rcva_len}};
  }

  void send_msg(const types::NetworkMessage &msg) const {
    if (sendto(sock, msg.first.c_str(), msg.first.length(), 0,
               reinterpret_cast<const sockaddr *>(&msg.second.first), msg.second.second) < 0)
      safe_throw();
  }

  UDPClient() : network::AbstractSocketWrapper(create_socket()) {}
};
}

#endif //RADIO_SK_SRC_UTILS_UDP_H_
