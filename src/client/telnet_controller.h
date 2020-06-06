#ifndef RADIO_SK_SRC_CLIENT_TELNET_CONTROLLER_H_
#define RADIO_SK_SRC_CLIENT_TELNET_CONTROLLER_H_

#include <poll.h>
#include "../utils/tcp.h"
#include "io.h"
#include "menu.h"

namespace telnet {
using __gnu_cxx::stdio_filebuf;
class Controller {
  tcp::TCPServer server_;
  menu::Menu menu_;
  pollfd pollfd_{};
  static constexpr size_t POLL_TIMEOUT_ON_KEY = 69;

  io::KeyType wait_for_key(int sock) {
    while (true) {
      io::TelnetIOHandler::display_lines(sock, menu_.render_lines());

      pollfd_.events = POLLIN;
      int ret = poll(&pollfd_, 1, POLL_TIMEOUT_ON_KEY);
      switch (ret) {
        case -1:throw exceptions::SocketException(errno);
        case 0:break;
        default:return io::TelnetIOHandler::get_key(sock);
      }
    }
  }

  [[noreturn]] void handle_one_connection(int sock) {
    pollfd_.fd = sock;

    io::TelnetIOHandler::negotiate(sock);
    while (true) {
      auto msg = wait_for_key(sock);

      switch (msg) {
        case io::KEY_UP:menu_.on_down();
          break;
        case io::KEY_DOWN:menu_.on_up();
          break;
        case io::ENTER:menu_.on_enter();
          break;
        default:break;
      }
    }
  }

 public:
  Controller(uint16_t telnet_port, State &state, const sender::DiscoverSender &send)
      : server_(telnet_port), menu_(state, send) {}

  [[noreturn]] void operator()() {
    while (true) {
      try {
        int sock = server_.get_accepted_connection_sock();
        handle_one_connection(sock);
      } catch (const exceptions::IOException &) {
        continue;
      }
    }
  }
};
}

#endif //RADIO_SK_SRC_CLIENT_TELNET_CONTROLLER_H_
