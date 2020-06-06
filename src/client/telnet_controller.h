#ifndef RADIO_SK_SRC_CLIENT_TELNET_CONTROLLER_H_
#define RADIO_SK_SRC_CLIENT_TELNET_CONTROLLER_H_

#include "../utils/tcp.h"
#include "io.h"
#include "menu.h"

namespace telnet {
using __gnu_cxx::stdio_filebuf;
class Controller {
  tcp::TCPServer server_;
  menu::Menu menu_;

  [[noreturn]] void handle_one_connection(int sock) {
    io::TelnetIOHandler::negotiate(sock);
    while (true) {
      auto lines = menu_.render_lines();
      io::TelnetIOHandler::display_lines(sock, lines);
      auto msg = io::TelnetIOHandler::get_key(sock);
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
