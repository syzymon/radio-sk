#include <string>
#include <optional>

#include "utils/icy.h"
#include "proxy.h"

[[noreturn]] void run_proxy(const std::string& host,
                            const std::string& resource,
                            const std::string& port,
                            bool want_meta,
                            size_t timeout,
                            std::optional<std::uint16_t> udp_port,
                            const std::optional<std::string>& multicast_addr,
                            size_t client_timeout) {
  if (!udp_port) {
    auto proxy = proxy::StdoutProxy();
    const auto &[on_audio, on_meta] = proxy.listeners();

    auto recv = icy::ICYReceiver(on_audio, on_meta,
                                 host, port, resource, want_meta, timeout);
    recv.stream_content();
  } else {
    auto proxy = proxy::UDPProxy(*udp_port, multicast_addr, client_timeout);
    const auto &[on_audio, on_meta] = proxy.listeners();

    auto recv = icy::ICYReceiver(on_audio, on_meta,
                                 host, port, resource, want_meta, timeout);
    recv.stream_content();
  }
}

