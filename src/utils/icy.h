#ifndef RADIO_SK_SRC_UTILS_ICY_H_
#define RADIO_SK_SRC_UTILS_ICY_H_

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/newline.hpp>
#include <sstream>

#include "types.h"
#include "receiver.h"
#include "tcp.h"

namespace icy {
constexpr std::string_view CRLF = "\r\n";
constexpr char COLON = ':';
using HeaderEntry = std::pair<std::string, std::string>;
using Headers = std::unordered_map<std::string, std::string>;

class Receiver : public AbstractReceiver {
  using super = AbstractReceiver;
  static constexpr size_t BUF_SIZE = 65536;
  static constexpr size_t CHUNK_SIZE = 8192;
  static constexpr size_t META_LEN_MULTIPLIER = 16;
  char buffer[BUF_SIZE]{};
  tcp::TCPClient client_;
  std::iostream &tcp_stream;

  const std::string &addr;
  const std::string &port;
  const std::string &resource;
  const bool meta;
  std::string &current_stream_name;

  void send_req() {
    tcp_stream << "GET " + resource + " HTTP/1.1" << CRLF;
    tcp_stream << "User-Agent: sikradio/2.13.7 (linux-gnu)" << CRLF;
    tcp_stream << "Accept: */*" << CRLF;
    tcp_stream << "Accept-Encoding: identity" << CRLF;
    tcp_stream << "Host: " << addr << COLON << port << CRLF;
    tcp_stream << "Connection: Keep-Alive" << CRLF;
    if (meta) tcp_stream << "Icy-MetaData: 1" << CRLF;
    tcp_stream << CRLF;
  }

  static HeaderEntry parse_header(const std::string &header_ln) {
    std::istringstream iss(header_ln);
    std::string key, value;
    std::getline(iss, key, COLON);
    std::getline(iss, value);
    return {key, value};
  }

  static Headers parse_headers(std::istream &response) {
    Headers res;
    std::string response_ln;
    std::getline(response, response_ln);
    response_ln.pop_back();

    std::istringstream iss(response_ln);
    std::string _;
    uint16_t status_code;
    iss >> _ >> status_code;
    if (status_code != 200)
      throw exceptions::RadioException("Bad status code: " + std::to_string(status_code));

    while (std::getline(response, response_ln) && response_ln.length() > 1) {
      response_ln.pop_back();
      res.insert(parse_header(response_ln));
    }
    return res;
  }

  [[noreturn]] void read_audio_metadata_(std::istream &response, size_t metaint) {
    while (true) {
      response.read(buffer, metaint);
      on_audio(std::string(buffer, buffer + metaint));
      size_t meta_len = response.get() * META_LEN_MULTIPLIER;
      if (meta_len > 0) {
        response.read(buffer, meta_len);
        on_metadata(std::string(buffer, buffer + meta_len));
      }
    }
  }

  [[noreturn]] void read_audio_only_(std::istream &response) {
    while (true) {
      response.read(buffer, CHUNK_SIZE);
      on_audio(std::string(buffer, buffer + CHUNK_SIZE));
    }
  }

 public:
  Receiver() = delete;

  Receiver(const types::Listener &on_audio,
           const types::Listener &on_metadata,
           const std::string &addr,
           const std::string &port,
           const std::string &resource,
           bool want_metadata,
           types::seconds_t timeout,
           std::string &current_stream_name) :
      super(on_audio, on_metadata), client_(addr, port, timeout), tcp_stream(client_.get_stream()),
      addr(addr), port(port), resource(resource), meta(want_metadata), current_stream_name(current_stream_name) {}

  [[noreturn]] void operator()() override {
    send_req();
    Headers h = parse_headers(tcp_stream);
    if (h.find("icy-name") != h.end())
      current_stream_name = h["icy-name"];

    try {
      size_t metaint = std::strtoul(h.at("icy-metaint").c_str(), nullptr, 0);
      if (!meta)
        throw exceptions::RadioException("Didn't ask for metadata, but anyway sent.");
      read_audio_metadata_(tcp_stream, metaint);
    } catch (const std::out_of_range &e) {
      read_audio_only_(tcp_stream);
    }
  }
};
}

#endif //RADIO_SK_SRC_UTILS_ICY_H_
