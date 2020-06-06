#include <string>
#include <optional>
#include <memory>

#include "utils/icy.h"
#include "proxy.h"

namespace proxy {
class Main {
  std::string stream_name_;
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
      recv_(proxy_->on_audio(), proxy_->on_metadata(), host, port, resource, want_meta, timeout, stream_name_) {}

  Main(const std::string &host,
       const std::string &resource,
       const std::string &port,
       bool want_meta,
       size_t timeout,
       uint16_t udp_port,
       const std::optional<std::string> &multicast_addr,
       size_t client_timeout) :
      stream_name_("undefined"),
      proxy_(std::make_unique<UDPProxy>(udp_port, multicast_addr, client_timeout, stream_name_)),
      recv_(proxy_->on_audio(), proxy_->on_metadata(), host, port, resource, want_meta, timeout, stream_name_) {}

  [[noreturn]] void main() {
    recv_();
  }
};
}

