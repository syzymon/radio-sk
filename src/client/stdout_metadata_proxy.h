#ifndef RADIO_SK_SRC_CLIENT_STDOUT_METADATA_PROXY_H_
#define RADIO_SK_SRC_CLIENT_STDOUT_METADATA_PROXY_H_

#include "../utils/proxy.h"
#include "state.h"

namespace proxy {
class StdoutMetadataProxy : public RadioProxy {
  State &state_;

  static std::string parse_metadata(const std::string &str) {
    static constexpr char QUOTE = '\'';
    size_t first_quote = str.find_first_of(QUOTE);
    size_t last_quote = str.find_last_of(QUOTE);
    return str.substr(first_quote + 1, last_quote - first_quote - 1);
  }
 public:
  StdoutMetadataProxy() = delete;

  explicit StdoutMetadataProxy(State &state) : state_(state) {}

  void output_audio(const types::buffer_t &audio) override {
    std::cout << audio;
  }

  void output_meta(const types::buffer_t &meta) override {
    state_.set_current_song(parse_metadata(meta));
  }
};
}

#endif //RADIO_SK_SRC_CLIENT_STDOUT_METADATA_PROXY_H_
