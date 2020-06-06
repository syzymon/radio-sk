#ifndef RADIO_SK_SRC_CLIENT_STATE_H_
#define RADIO_SK_SRC_CLIENT_STATE_H_

#include <optional>
#include "../utils/types.h"
#include "proxies_pool.h"

class State {
  std::mutex mtx_;
  std::optional<types::addr_t> current_proxy;
  std::optional<std::string> current_song;
  types::time_point_t last_proxy_message;
  pool::ProxiesPool available_proxies;
  const types::seconds_t proxy_timeout;

 public:
  State() = delete;

  explicit State(types::seconds_t timeout) : proxy_timeout(timeout) {}

  const std::optional<types::addr_t> &get_current_client_addr() {
    types::lock_t lock(mtx_);
    types::time_point_t now = std::chrono::steady_clock::now();
    if (current_proxy
        && std::chrono::duration_cast<std::chrono::seconds>(now - last_proxy_message).count() >=
            static_cast<int64_t>(proxy_timeout)) {
      available_proxies.del(*current_proxy);
      current_proxy = std::nullopt;
      current_song = std::nullopt;
    }
    return current_proxy;
  }

  void set_current_proxy_addr(const types::addr_t &addr) {
    types::lock_t lock(mtx_);
    last_proxy_message = std::chrono::steady_clock::now();
    current_proxy = addr;
  }

  void set_last_msg_time() { // Unlocked since only UDP thread can read/modify it.
    last_proxy_message = std::chrono::steady_clock::now();
  }

  void add_proxy(const types::addr_t &addr, const std::string &meta) {
    available_proxies.add_proxy(addr, meta);
  }

  void set_current_song(const std::string &song_title) {
    types::lock_t lock(mtx_);
    current_song = song_title;
  }

  std::optional<std::string> get_current_song() {
    types::lock_t lock(mtx_);
    return current_song;
  }

  std::map<pool::UniqueAddressKey, std::string> get_current_proxies_view() {
    return available_proxies.get_proxies_view();
  }
};

#endif //RADIO_SK_SRC_CLIENT_STATE_H_
