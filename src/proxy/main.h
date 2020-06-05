#include <string>
#include <optional>
#include <memory>

#include "utils/icy.h"
#include "proxy.h"

namespace proxy {
class Main {
  std::unique_ptr<RadioProxy> proxy_;
  icy::Receiver recv_;

 public:
  Main() = delete;

  Main(const std::string &host,
       const std::string &resource,
       const std::string &port,
       bool want_meta,
       size_t timeout) :
      proxy_(std::make_unique<StdoutProxy>()),
      recv_(proxy_->on_audio(), proxy_->on_metadata(), host, port, resource, want_meta, timeout) {}

  Main(const std::string &host,
       const std::string &resource,
       const std::string &port,
       bool want_meta,
       size_t timeout,
       uint16_t udp_port,
       const std::optional<std::string> &multicast_addr,
       size_t client_timeout) :
      proxy_(std::make_unique<UDPProxy>(udp_port, multicast_addr, client_timeout)),
      recv_(proxy_->on_audio(), proxy_->on_metadata(), host, port, resource, want_meta, timeout) {}

  [[noreturn]] void main() {
    recv_();
  }
};
}

