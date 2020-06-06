#ifndef RADIO_SK_SRC_UTILS_TCP_H_
#define RADIO_SK_SRC_UTILS_TCP_H_

#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ext/stdio_filebuf.h>
#include <iostream>

#include "types.h"
#include "socket_wrapper.h"
#include "../utils/exceptions.h"
#include "addr.h"

namespace tcp {
using __gnu_cxx::stdio_filebuf;
class TCPClient : public network::AbstractSocketWrapper {
  stdio_filebuf<char> buf_;
  std::iostream sock_stream_;

  static int initialize_socket(const std::string &addr, const std::string &port,
                               types::seconds_t timeout) {
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
      throw exceptions::SocketException(errno);

    timeval tv{
        .tv_sec=static_cast<__time_t>(timeout),
        .tv_usec=0
    };
    if ((setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv)) < 0) {
      close(sock);
      throw exceptions::SocketException(errno);
    }

    addrinfo addr_hints{
        .ai_flags = 0,
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = IPPROTO_TCP,
        .ai_addrlen = 0,
        .ai_addr = nullptr,
        .ai_canonname = nullptr,
        .ai_next = nullptr
    }, *addr_result;

    int rc = getaddrinfo(addr.c_str(), port.c_str(), &addr_hints, &addr_result);
    if (rc != 0) {
      close(sock);
      throw exceptions::AddressException(gai_strerror(rc));
    }

    if (connect(sock, addr_result->ai_addr, addr_result->ai_addrlen) != 0) {
      close(sock);
      throw exceptions::SocketException(errno);
    }
    freeaddrinfo(addr_result);
    return sock;
  }

 public:
  TCPClient() = delete;

  TCPClient(const std::string &addr, const std::string &port, types::seconds_t timeout) :
      AbstractSocketWrapper(initialize_socket(addr, port, timeout)),
      buf_(sock, std::ios::in | std::ios::out), sock_stream_(&buf_) {}

  std::iostream &get_stream() {
    return sock_stream_;
  }
};

class TCPServer : public network::AbstractSocketWrapper {
  static constexpr size_t QUEUE_LEN = 5;

  static int initialize_socket(uint16_t port) {
    std::cerr << "Starting!\n";
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
      throw exceptions::SocketException(errno);
    }

    const auto[server_address, addr_sz] = addr::ip_port_to_addr(port);
    if (bind(sock, reinterpret_cast<const sockaddr *>(&server_address), addr_sz) < 0) {
      close(sock);
      throw exceptions::SocketException(errno);
    }

    if (listen(sock, QUEUE_LEN) < 0) {
      close(sock);
      throw exceptions::SocketException(errno);
    }
    std::cerr << "Socket created!\n";
    return sock;
  }

 public:
  TCPServer() = delete;

  explicit TCPServer(uint16_t server_port)
      : network::AbstractSocketWrapper(initialize_socket(server_port)) {}

  int get_accepted_connection_sock() {
    sockaddr_in client_address{};
    socklen_t client_address_len = sizeof(client_address);

    int msg_sock = accept(sock, reinterpret_cast<sockaddr *>(&client_address), &client_address_len);
    if (msg_sock < 0)
      safe_throw();
    std::cerr << "Received connection\n";
    return msg_sock;
  }
};
}

#endif //RADIO_SK_SRC_UTILS_TCP_H_
