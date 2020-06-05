#ifndef RADIO_SK_SRC_PROXY_PROXY_H_
#define RADIO_SK_SRC_PROXY_PROXY_H_

#include <thread>
#include "utils/types.h"
#include "content_dispatcher.h"
#include "proxy_server.h"
#include "audio_sender.h"
#include "utils/proxy.h"

class UDPProxy : public proxy::RadioProxy {
 private:
  ClientsHandlerSocket socket_;
  pool::ClientsPool pool_;
  ContentSender sender_;
  ProxyServer server_;
  ContentDispatcher dispatcher_;
  std::thread proxy_srv_worker_;

 public:
  UDPProxy() = delete;

  explicit UDPProxy(uint16_t server_port,
                    const std::optional<std::string> &multi, types::seconds_t secs_timeout)
      : socket_(server_port, multi), pool_(secs_timeout), sender_(socket_, pool_),
        server_(socket_, pool_), dispatcher_(sender_), proxy_srv_worker_(std::ref(server_)) {
    proxy_srv_worker_.detach();
  }

  void output_audio(const types::buffer_t &audio) override {
    dispatcher_.send_audio(audio);
  }

  void output_meta(const types::buffer_t &meta) override {
    dispatcher_.send_metadata(meta);
  }
};

#endif //RADIO_SK_SRC_PROXY_PROXY_H_
