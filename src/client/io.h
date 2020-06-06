#ifndef RADIO_SK_SRC_CLIENT_IO_H_
#define RADIO_SK_SRC_CLIENT_IO_H_

#include "../utils/exceptions.h"

namespace io {
enum KeyType {
  KEY_UP,
  KEY_DOWN,
  ENTER,
  UNKNOWN
};
class TelnetIOHandler {
  static KeyType get_arrow(const std::string &buffer, ssize_t buffer_len) {
    if (buffer[0] != '\x1B' || buffer_len != 3 || buffer[1] != '\x5B')
      return UNKNOWN;
    else if (buffer[2] == 'B')
      return KEY_UP;
    else if (buffer[2] == 'A')
      return KEY_DOWN;
    return UNKNOWN;
  }

  static KeyType get_enter(const std::string &buffer, ssize_t buffer_len) {
    if (buffer_len == 2 && buffer[0] == '\x0D' && buffer[1] == '\0')
      return ENTER;
    return UNKNOWN;
  }

 public:
  static void negotiate(int fd) {
    static const char *nego = "\377\375\042\377\373\001";
    if (write(fd, nego, 6) != 6) {
      throw exceptions::IOException("negotiation failed");
    }
  }
  static KeyType get_key(int fd) {
    static char k_buffer[6];
    ssize_t len = read(fd, &k_buffer, sizeof(k_buffer));
    if (len <= 0) {
      throw exceptions::IOException("read key failure");
    }
    char fst = k_buffer[0];
    switch (fst) {
      case '\x1B':return get_arrow(std::string(k_buffer), len);
      case '\x0D':return get_enter(std::string(k_buffer), len);
      default:return UNKNOWN;
    }
  };

  static void display_lines(int fd, const std::vector<std::string> &txt) {
    static const char flush[] = {0x1B, 'c', 0};
    if (write(fd, flush, 2) != 2) {
      throw exceptions::IOException("invalid flush");
    }
    for (const auto &v : txt) write_line(fd, v);
  }

  static void write_line(int fd, const std::string &line) {
    std::string line_terminated = line + "\r\n";
    if (write(fd, line_terminated.c_str(), line_terminated.length())
        != static_cast<ssize_t>(line_terminated.length())) {
      throw exceptions::IOException("");
    }
  }
};
}

#endif //RADIO_SK_SRC_CLIENT_IO_H_
