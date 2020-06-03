#ifndef RADIO_SK_SRC_PROXY_CLIENTS_POOL_H_
#define RADIO_SK_SRC_PROXY_CLIENTS_POOL_H_

#include <mutex>
#include <chrono>
#include <boost/functional/hash.hpp>
#include <netinet/in.h>

namespace clients_pool {
using time_t = std::chrono::steady_clock::time_point;
using UniqueAddressKey = std::pair<uint32_t, uint16_t>;
using ClientData = time_t;
using ClientEntry = std::pair<UniqueAddressKey, ClientData>;

UniqueAddressKey decode_addr(const types::addr_t &addr) {
  return {addr.first.sin_addr.s_addr, addr.first.sin_port};
}

types::addr_t encode_addr(const UniqueAddressKey &key) {
  sockaddr_in addr = {
      .sin_family=AF_INET,
      .sin_port=key.second,
      .sin_addr={
          .s_addr=key.first
      },
      .sin_zero{}
  };

  socklen_t len = sizeof addr;
  // TODO: test this encoding
  return {addr, len};
}

class ClientsPool {
  std::mutex mtx_;
  types::seconds_t timeout_;
  std::unordered_map<UniqueAddressKey, ClientData, boost::hash<UniqueAddressKey>> clients_data_;

  void add_update_client_(const UniqueAddressKey &key) {
    clients_data_[key] = std::chrono::steady_clock::now();
  }

  bool client_sendable(const ClientEntry &client) const {
    time_t now = std::chrono::steady_clock::now();
    types::seconds_t diff = std::chrono::duration_cast<std::chrono::seconds>(now - client.second).count();
    return diff < timeout_;
  }

 public:
  ClientsPool() = delete;

  explicit ClientsPool(types::seconds_t secs_timeout) : timeout_(secs_timeout) {}

  void add_client(const types::addr_t &new_client_addr) {
    const auto key = decode_addr(new_client_addr);

    std::unique_lock<std::mutex> lock(mtx_);
    add_update_client_(key);
  }

  void update_client_time(const types::addr_t &client_addr) {
    const auto key = decode_addr(client_addr);

    std::unique_lock<std::mutex> lock(mtx_);
    add_update_client_(key);
  }

  [[nodiscard]] std::vector<types::addr_t> sendable_clients() {
    std::vector<types::addr_t> res;

    std::unique_lock<std::mutex> lock(mtx_);
    std::vector<ClientEntry> tmp;
    std::copy_if(clients_data_.begin(), clients_data_.end(), std::back_inserter(tmp),
                 [this](const ClientEntry &client) { return client_sendable(client); });
    std::transform(tmp.begin(), tmp.end(), std::back_inserter(res),
                   [](const ClientEntry &entry) { return encode_addr(entry.first); });
    return res;
  }
};
}

#endif //RADIO_SK_SRC_PROXY_CLIENTS_POOL_H_
