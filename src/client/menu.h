#ifndef RADIO_SK_SRC_CLIENT_MENU_H_
#define RADIO_SK_SRC_CLIENT_MENU_H_

#include <string_view>
#include <string>
#include <utility>
#include "state.h"
#include "sender.h"
#include "../utils/address_pool.h"

namespace menu {
class Menu {
  State &state_;
  const sender::DiscoverSender &send_;
  std::optional<std::string> current_proxy_name_;

  class MenuItem {
   public:
    virtual void on_click() = 0;
    [[nodiscard]] virtual std::string caption() const = 0;
  };

  class SearchForProxies : public MenuItem {
    const sender::DiscoverSender &send_;
    explicit SearchForProxies(const sender::DiscoverSender &send) : send_(send) {}
   public:
    static SearchForProxies &getInstance(const sender::DiscoverSender &send) {
      static SearchForProxies theInstance(send);
      return theInstance;
    }

    void on_click() override {
      send_.discover_all();
    }

    [[nodiscard]] std::string caption() const override {
      static const std::string text = "Szukaj posrednika";
      return text;
    }
  };

  class EndProgram : public MenuItem {
    EndProgram() = default;
   public:
    static EndProgram &getInstance() {
      static EndProgram theInstance;
      return theInstance;
    }

    void on_click() override {
      exit(0);
    }

    [[nodiscard]] std::string caption() const override {
      static const std::string text = "Koniec";
      return text;
    }
  };

  class ProxyItem : public MenuItem {
    Menu &menu_;
    std::string metadata_;
    const pool::UniqueAddressKey addr_;

   public:
    ProxyItem(std::string meta, pool::UniqueAddressKey key, Menu &menu)
        : menu_(menu), metadata_(std::move(meta)), addr_(std::move(key)) {
      static const std::string prefix = "Posrednik ";
      metadata_ = prefix + metadata_;
    }

    const pool::UniqueAddressKey &get_addr() {
      return addr_;
    }

    void on_click() override {
      static const auto addr_encoded = pool::encode_addr(addr_);
      menu_.state_.set_current_proxy_addr(addr_encoded);
      menu_.send_.discover_single(addr_encoded);
      menu_.current_proxy_name_ = metadata_;
    }

    [[nodiscard]] std::string caption() const override {
      return metadata_;
    }
  };

  SearchForProxies search_;
  EndProgram end_;
  std::list<ProxyItem> proxies_;
  std::set<pool::UniqueAddressKey> current_keys_;
  MenuItem *currently_selected_;

  void update_proxies() {
    std::map<pool::UniqueAddressKey, std::string> curr_addr_view = state_.get_current_proxies_view();
    // Erase outdated proxies from our menu list.
    for (auto it = proxies_.begin(); it != proxies_.end();) {
      auto addr = it->get_addr();
      if (curr_addr_view.find(addr) == curr_addr_view.end()) {
        current_keys_.erase(addr);
        it = proxies_.erase(it);
      } else
        ++it;
    }
    // Populate new values into the list.
    for (const auto &[k, v] : curr_addr_view) {
      if (current_keys_.find(k) == current_keys_.end()) {
        current_keys_.insert(k);
        proxies_.emplace_back(v, k, *this);
      }
    }
  }

  std::list<ProxyItem>::iterator find_currently_selected_() {
    return std::find_if(proxies_.begin(), proxies_.end(), [this](const ProxyItem &item) {
      return currently_selected_ == &item;
    });
  }

  std::string caption_with_selected(MenuItem *item) {
    static const std::string asterisk = " *";
    std::string res = item->caption();
    if (currently_selected_ == item)
      res += asterisk;
    return res;
  }
 public:
  Menu(State &state, const sender::DiscoverSender &sender) : state_(state),
                                                             send_(sender),
                                                             search_(SearchForProxies::getInstance(send_)),
                                                             end_(EndProgram::getInstance()),
                                                             currently_selected_(&search_) {
    update_proxies();
  }

  void on_enter() {
    if (currently_selected_ != nullptr)
      currently_selected_->on_click();
  }

  void on_up() {
    if (currently_selected_ == &search_) {
      currently_selected_ = &end_;
    } else if (currently_selected_ == &end_) {
      if (proxies_.empty())
        currently_selected_ = &search_;
      else
        currently_selected_ = &proxies_.back();
    } else {
      auto current_list_iter = find_currently_selected_();
      if (current_list_iter == proxies_.begin())
        currently_selected_ = &search_;
      else
        currently_selected_ = &*std::prev(current_list_iter);
    }
  }

  void on_down() {
    if (currently_selected_ == &end_) {
      currently_selected_ = &search_;
    } else if (currently_selected_ == &search_) {
      if (proxies_.empty()) {
        currently_selected_ = &end_;
      } else
        currently_selected_ = &*proxies_.begin();
    } else {
      auto current_list_iter = find_currently_selected_();
      if (std::distance(current_list_iter, proxies_.end()) == 1)
        currently_selected_ = &end_;
      else
        currently_selected_ = &*std::next(current_list_iter);
    }
  }

  std::vector<std::string> render_lines() {
    update_proxies();
    if (current_proxy_name_ && !state_.get_current_client_addr()) {
      current_proxy_name_ = std::nullopt;
      currently_selected_ = &search_;
    }

    std::vector<std::string> res;
    res.push_back(caption_with_selected(&search_));
    for (auto &proxy_item : proxies_)
      res.push_back(caption_with_selected(&proxy_item));
    res.push_back(caption_with_selected(&end_));
    // Separator
    res.emplace_back();
    auto current_song = state_.get_current_song();
    if (current_proxy_name_) {
      res.push_back("Sluchasz: " + *current_proxy_name_);
      if (current_song)
        res.push_back(*current_song);
    }
    return res;
  }
};
}

#endif //RADIO_SK_SRC_CLIENT_MENU_H_
