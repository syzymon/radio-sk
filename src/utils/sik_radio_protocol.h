#ifndef RADIO_SK_SRC_UTILS_SIK_RADIO_PROTOCOL_H_
#define RADIO_SK_SRC_UTILS_SIK_RADIO_PROTOCOL_H_

#include <utility>
#include <cstdint>
#include <sstream>

#include "types.h"

namespace srp {
using IntegerField = uint16_t;
constexpr IntegerField DISCOVER = 1;
constexpr IntegerField IAM = 2;
constexpr IntegerField KEEPALIVE = 3;
constexpr IntegerField AUDIO = 4;
constexpr IntegerField METADATA = 6;

class Message {
  static constexpr uint16_t MAX_MSG_LEN = std::numeric_limits<uint16_t>::max();
  IntegerField type_;
  types::buffer_t body_;

 public:
  Message() = delete;

  explicit Message(IntegerField type, types::buffer_t body = "") : type_(type), body_(std::move(body)) {}

  [[nodiscard]] IntegerField type() const { return type_; }

  [[nodiscard]] types::buffer_t body() const { return body_; }

  [[nodiscard]] types::buffer_t encode() const {
    std::ostringstream ss;
    uint16_t converted_type = htons(type_);
    uint16_t converted_len = htons(body_.length());
    ss.write(reinterpret_cast<const char *>(&converted_type), 2);
    ss.write(reinterpret_cast<const char *>(&converted_len), 2);
    ss << body_;
    return ss.str();
  }

  static Message decode(const types::buffer_t &msg_str) {
    std::istringstream ss(msg_str);
    uint16_t type, len;
    ss.read(reinterpret_cast<char *>(&type), 2);
    ss.read(reinterpret_cast<char *>(&len), 2);

    std::ostringstream rest;
    rest << ss.rdbuf();
    std::string body = rest.str();
    return Message(ntohs(type), body);
  }
};
}
#endif //RADIO_SK_SRC_UTILS_SIK_RADIO_PROTOCOL_H_
