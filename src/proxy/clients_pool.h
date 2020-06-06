#ifndef RADIO_SK_SRC_PROXY_CLIENTS_POOL_H_
#define RADIO_SK_SRC_PROXY_CLIENTS_POOL_H_

#include <mutex>
#include <chrono>
#include <boost/functional/hash.hpp>
#include <netinet/in.h>
#include "utils/address_pool.h"

namespace pool {
using ClientData = types::time_point_t;
using ClientEntry = std::pair<UniqueAddressKey, ClientData>;

class ClientsPool : public pool::SynchronizedAddressPool<ClientData> {
  types::seconds_t timeout_;

  static ClientData get_current_time() {
    return std::chrono::steady_clock::now();
  }

  bool client_sendable(const ClientEntry &client) const {
    types::time_point_t now = std::chrono::steady_clock::now();
    types::seconds_t diff = std::chrono::duration_cast<std::chrono::seconds>(now - client.second).count();
    return diff < timeout_;
  }

 public:
  ClientsPool() = delete;

  explicit ClientsPool(types::seconds_t secs_timeout) : timeout_(secs_timeout) {}

  void add_client(const types::addr_t &new_client_addr) {
    put(new_client_addr, get_current_time());
  }

  void update_client_time(const types::addr_t &client_addr) {
    put(client_addr, get_current_time());
  }

  [[nodiscard]] std::vector<types::addr_t> sendable_clients() {
    std::vector<types::addr_t> res;

    auto tmp = filtered_entries([this](const ClientEntry &client) { return client_sendable(client); });
    std::transform(tmp.begin(), tmp.end(), std::back_inserter(res),
                   [](const ClientEntry &entry) { return encode_addr(entry.first); });
    return res;
  }
};
}

#endif //RADIO_SK_SRC_PROXY_CLIENTS_POOL_H_
