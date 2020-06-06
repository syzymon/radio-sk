#ifndef RADIO_SK_SRC_PROXY_CONTENT_DISPATCHER_H_
#define RADIO_SK_SRC_PROXY_CONTENT_DISPATCHER_H_

#include "../utils/sik_radio_protocol.h"
#include "audio_sender.h"

class ContentDispatcher {
  const ContentSender &sender_;

 public:
  ContentDispatcher() = delete;

  explicit ContentDispatcher(const ContentSender &sender) : sender_(sender) {}

  void send_audio(const types::buffer_t &data) {
    auto msg = srp::Message(srp::AUDIO, data);
    sender_.send_to_clients(msg);
  }

  void send_metadata(const types::buffer_t &data) {
    auto msg = srp::Message(srp::METADATA, data);
    sender_.send_to_clients(msg);
  }
};

#endif //RADIO_SK_SRC_PROXY_CONTENT_DISPATCHER_H_
