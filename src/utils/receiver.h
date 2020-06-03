#ifndef RADIO_SK_SRC_UTILS_RECEIVER_H_
#define RADIO_SK_SRC_UTILS_RECEIVER_H_

#include <functional>
#include <utility>

#include "types.h"

class AbstractReceiver {
 protected:
  const types::Listener &on_audio;
  const types::Listener &on_metadata;
 public:
  AbstractReceiver() = delete;

  AbstractReceiver(const types::Listener &on_audio, const types::Listener &on_metadata) :
      on_audio(on_audio), on_metadata(on_metadata) {}

  virtual void stream_content() = 0;
  virtual ~AbstractReceiver() = default;
};

#endif //RADIO_SK_SRC_UTILS_RECEIVER_H_
