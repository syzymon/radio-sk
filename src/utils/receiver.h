#ifndef RADIO_SK_SRC_UTILS_RECEIVER_H_
#define RADIO_SK_SRC_UTILS_RECEIVER_H_

#include <functional>
#include <utility>

#include "types.h"
#include "proxy.h"

class AbstractReceiver {
 protected:
  types::Listener on_audio;
  types::Listener on_metadata;
 public:
  AbstractReceiver() = delete;

  AbstractReceiver(types::Listener on_audio, types::Listener on_meta) :
      on_audio(std::move(on_audio)),
      on_metadata(std::move(on_meta)) {}

  AbstractReceiver(proxy::RadioProxy &p) {
    const auto &[aud, met] = p.listeners();
    on_audio = aud;
    on_metadata = met;
  }

  virtual void stream_content() = 0;
  virtual ~AbstractReceiver() = default;
};

#endif //RADIO_SK_SRC_UTILS_RECEIVER_H_
