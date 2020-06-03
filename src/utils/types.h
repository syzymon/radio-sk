#ifndef RADIO_SK_SRC_UTILS_TYPES_H_
#define RADIO_SK_SRC_UTILS_TYPES_H_

#include <string>
#include <unistd.h>
#include <netinet/in.h>

namespace types {
using buffer_t = std::string;
using seconds_t = size_t;
using addr_t = std::pair<sockaddr_in, socklen_t>;
using NetworkMessage = std::pair<buffer_t, addr_t>;
using Listener = std::function<void(const std::string&)>;
}

#endif //RADIO_SK_SRC_UTILS_TYPES_H_
