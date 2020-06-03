//
// Created by syzymon on 03.06.2020.
//

#ifndef RADIO_SK_SRC_UTILS_PROXY_H_
#define RADIO_SK_SRC_UTILS_PROXY_H_

#include <iostream>
#include <functional>
#include "types.h"


namespace proxy {
class RadioProxy {
 public:
  virtual void output_audio(const types::buffer_t &audio) = 0;
  virtual void output_meta(const types::buffer_t &meta) = 0;
  virtual std::pair <types::Listener, types::Listener> listeners() {
    return {std::bind(&RadioProxy::output_audio, this, std::placeholders::_1),
            std::bind(&RadioProxy::output_meta, this, std::placeholders::_1)};
  }

  virtual ~RadioProxy() = default;
};

class StdoutProxy : public RadioProxy {
 public:
  void output_audio(const types::buffer_t &audio) override {
    std::cout << audio;
  }

  void output_meta(const types::buffer_t &meta) override {
    std::cerr << meta;
  }
};
}
#endif //RADIO_SK_SRC_UTILS_PROXY_H_
