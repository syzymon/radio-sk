#include <boost/program_options.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>

#include "proxy/main.h"

namespace po = boost::program_options;
template<typename T>
std::optional<T> boost_to_std_opt(const boost::optional<T> &opt) {
  return opt ? std::optional<T>(*opt) : std::nullopt;
}

/*
 Program radio-proxy uruchomiony bez parametru -P ma działać wg specyfikacji części A zadania.
 Program radio-proxy uruchomiony z parametrem -P ma działać wg specyfikacji części B zadania.
 Program radio-proxy uruchomiony z parametrem -P, ale bez parametru -B ma odbierać komunikaty
 UDP wysyłane na wszystkie lokalne adresy jednostkowe i adres 255.255.255.255.
 Program radio-proxy uruchomiony z parametrami -P i-B ma odbierać komunikaty UDP
 wysyłane na wszystkie lokalne adresy jednostkowe, adres 255.255.255.255 i adres podany w parametrze -B.
 */
int main(int argc, char *argv[]) {
  po::options_description desc("parameters");
  desc.add_options()
      (",h", po::value<std::string>()->required(), "RADIO SERVER HOST")
      (",r", po::value<std::string>()->required(), "RESOURCE")
      (",p", po::value<std::string>()->required(), "PORT")

      (",m", po::value<std::string>()->default_value("no"), "METADATA")
      (",t", po::value<size_t>()->default_value(5), "TIMEOUT")

      (",P", po::value<boost::optional<uint16_t>>()->default_value(boost::none), "UDP PORT")
      (",B", po::value<boost::optional<std::string>>()->default_value(boost::none), "MULTICAST ADDR")
      (",T", po::value<size_t>()->default_value(5), "CLIENT TIMEOUT");

  po::variables_map var_map;
  try {
    po::store(po::parse_command_line(argc, argv, desc), var_map);
    po::notify(var_map);
  } catch (po::error &err) {
    std::cerr << err.what() << std::endl;
    exit(1);
  }

  std::string host = var_map["-h"].as<std::string>();
  std::string resource = var_map["-r"].as<std::string>();
  std::string port = var_map["-p"].as<std::string>();

  bool want_meta = var_map["-m"].as<std::string>() == "yes";
  size_t timeout = var_map["-t"].as<size_t>();

  auto udp_port = var_map["-P"].as<boost::optional<uint16_t>>();
  auto multicast_addr = var_map["-B"].as<boost::optional<std::string>>();
  size_t client_timeout = var_map["-T"].as<size_t>();

  run_proxy(host, resource, port, want_meta, timeout,
            boost_to_std_opt(udp_port), boost_to_std_opt(multicast_addr),
            client_timeout);
}
