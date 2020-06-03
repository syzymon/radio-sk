#ifndef RADIO_SK_SRC_UTILS_SIK_RADIO_PROTOCOL_H_
#define RADIO_SK_SRC_UTILS_SIK_RADIO_PROTOCOL_H_

#include <utility>
#include <cstdint>

#include "types.h"

namespace srp {
using IntegerField = uint16_t;
constexpr IntegerField DISCOVER = 1;
constexpr IntegerField IAM = 2;
constexpr IntegerField KEEPALIVE = 3;
constexpr IntegerField AUDIO = 4;
constexpr IntegerField METADATA = 6;

class Message {
  IntegerField type_;
  types::buffer_t body;

 public:
  Message() = delete;

  Message(IntegerField type, types::buffer_t body) : type_(type), body(std::move(body)) {}

  [[nodiscard]] IntegerField type() const { return type_; }

  [[nodiscard]] types::buffer_t encode() const {
    // TODO: implement
  }

  static const Message &decode(const types::buffer_t& msg_str) {
    // TODO: implement
  }
};



//const types::buffer_t encode(const Message &msg) {
//
//}
}
#endif //RADIO_SK_SRC_UTILS_SIK_RADIO_PROTOCOL_H_
