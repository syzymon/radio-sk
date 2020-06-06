#ifndef RADIO_SK_SRC_CLIENT_PROXIES_POOL_H_
#define RADIO_SK_SRC_CLIENT_PROXIES_POOL_H_

#include "../utils/address_pool.h"

namespace pool {
class ProxiesPool : public SynchronizedAddressPool<std::string> {
 public:
  void add_proxy(const types::addr_t &addr, const std::string &meta) {
    put(addr, meta);
  }

  std::map<pool::UniqueAddressKey, std::string> get_proxies_view() {
    auto entries = filtered_entries([]([[maybe_unused]]const Entry &entry) { return true; });
    return std::map<pool::UniqueAddressKey, std::string>(entries.begin(), entries.end());
  }
};
}

#endif //RADIO_SK_SRC_CLIENT_PROXIES_POOL_H_
