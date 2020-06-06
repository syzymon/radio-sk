#ifndef RADIO_SK_SRC_UTILS_TYPES_H_
#define RADIO_SK_SRC_UTILS_TYPES_H_

#include <string>
#include <functional>
#include <unistd.h>
#include <netinet/in.h>
#include <chrono>
#include <mutex>

namespace types {
using buffer_t = std::string;
using seconds_t = size_t;
using addr_t = std::pair<sockaddr_in, socklen_t>;
using NetworkMessage = std::pair<buffer_t, addr_t>;
using Listener = std::function<void(const std::string &)>;
using time_point_t = std::chrono::steady_clock::time_point;
using lock_t = std::unique_lock<std::mutex>;
}

#endif //RADIO_SK_SRC_UTILS_TYPES_H_
