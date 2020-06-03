#ifndef RADIO_SK_SRC_UTILS_EXCEPTIONS_H_
#define RADIO_SK_SRC_UTILS_EXCEPTIONS_H_

#include <exception>
#include <string>
#include <cstring>

namespace exceptions {
std::string errno_to_str(int err_num) {
  return "ERROR: " + std::string(strerror(err_num));
}

class RadioException : public std::exception {
  std::string msg;

 public:
  explicit RadioException(std::string &&msg) :
      msg(std::move(msg)) {}

  [[nodiscard]] const char *what() const noexcept override {
    return msg.c_str();
  }
};

class InvalidInputException : public RadioException {
 public:
  explicit InvalidInputException(std::string msg) :
      RadioException(std::move(msg)) {}
};

class SocketException : public RadioException {
 public:
  explicit SocketException(int err_num) :
      RadioException(errno_to_str(err_num)) {}
};

class AddressException : public RadioException {
 public:
  explicit AddressException(const std::string &msg) :
      RadioException("Invalid address: " + msg) {}
};
}

#endif //RADIO_SK_SRC_UTILS_EXCEPTIONS_H_
