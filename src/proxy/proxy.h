#ifndef RADIO_SK_SRC_PROXY_PROXY_H_
#define RADIO_SK_SRC_PROXY_PROXY_H_

#include <thread>
#include "utils/types.h"
#include "content_dispatcher.h"
#include "proxy_server.h"
#include "audio_sender.h"

namespace proxy {
class RadioProxy {
 public:
  virtual void output_audio(const types::buffer_t &audio) = 0;
  virtual void output_meta(const types::buffer_t &meta) = 0;
  virtual std::pair<types::Listener, types::Listener> listeners() {
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

class UDPProxy : public RadioProxy {
 private:
  ClientsHandlerSocket socket_;
  clients_pool::ClientsPool pool_;
  ContentSender sender_;
  ProxyServer server_;
  ContentDispatcher dispatcher_;

 public:
  UDPProxy() = delete;

  explicit UDPProxy(uint16_t server_port,
                    const std::optional<std::string> &multi, types::seconds_t secs_timeout)
      : socket_(server_port, multi), pool_(secs_timeout), sender_(socket_, pool_),
        server_(socket_, pool_), dispatcher_(sender_) {
    std::thread t1(std::ref(server_));
    t1.detach();
  }

  void output_audio(const types::buffer_t &audio) override {
    dispatcher_.send_audio(audio);
  }

  void output_meta(const types::buffer_t &meta) override {
    dispatcher_.send_metadata(meta);
  }
};
}

#endif //RADIO_SK_SRC_PROXY_PROXY_H_
