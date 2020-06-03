#ifndef RADIO_SK_SRC_CLIENT_MAIN_H_
#define RADIO_SK_SRC_CLIENT_MAIN_H_

#include "proxies_handler_socket.h"
#include "srp_receiver.h"
#include "utils/proxy.h"
#include "utils/addr.h"


namespace client{
void main() {
  auto sock = ProxiesHandlerSocket();

  auto proxy = proxy::StdoutProxy();
  const auto &[on_audio, on_meta] = proxy.listeners();

  types::addr_t addr = addr::ip_port_to_addr("0.0.0.0", 10001);

  auto recv = srp::Receiver(on_audio, on_meta, sock, addr);
  recv.stream_content();
}
}

#endif //RADIO_SK_SRC_CLIENT_MAIN_H_
