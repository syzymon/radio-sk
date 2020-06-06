#ifndef RADIO_SK_SRC_UTILS_ADDRESS_POOL_H_
#define RADIO_SK_SRC_UTILS_ADDRESS_POOL_H_

#include <mutex>
#include <utility>
#include <boost/functional/hash.hpp>
#include "types.h"

namespace pool {
using UniqueAddressKey = std::pair<uint32_t, uint16_t>;
UniqueAddressKey decode_addr(const types::addr_t &addr) {
  return {addr.first.sin_addr.s_addr, addr.first.sin_port};
}
static types::addr_t encode_addr(const UniqueAddressKey &key) {
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

template<typename ValueType>
class SynchronizedAddressPool {
 private:
  std::mutex mtx_;
  std::unordered_map<UniqueAddressKey, ValueType, boost::hash<UniqueAddressKey>> clients_data_;
 protected:
  using Entry = std::pair<UniqueAddressKey, ValueType>;

  virtual void put(const types::addr_t &key, const ValueType &val) {
    const auto k = decode_addr(key);

    types::lock_t lock(mtx_);
    clients_data_[k] = val;
  }

  virtual std::vector<Entry> filtered_entries(const std::function<bool(const Entry &)> &filter) {
    types::lock_t lock(mtx_);

    std::vector<Entry> res;
    std::copy_if(clients_data_.begin(), clients_data_.end(), std::back_inserter(res), filter);
    return res;
  }
 public:
  virtual void del(const types::addr_t &key) {
    const auto k = decode_addr(key);

    types::lock_t lock(mtx_);
    clients_data_.erase(k);
  }
};
}

#endif //RADIO_SK_SRC_UTILS_ADDRESS_POOL_H_
