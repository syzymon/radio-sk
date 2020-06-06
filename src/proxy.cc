#include <boost/program_options.hpp>
#include <csignal>

#include "proxy/main.h"

namespace po = boost::program_options;
template<typename T>
std::optional<T> pseudo_opt_to_std_opt(const T &val, const T &null_val) {
  return val != null_val ? std::optional<T>(val) : std::nullopt;
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
  std::signal(SIGINT, []([[maybe_unused]]int signal) { exit((0)); });

  po::options_description desc("parameters");
  desc.add_options()
      (",h", po::value<std::string>()->required(), "RADIO SERVER HOST")
      (",r", po::value<std::string>()->required(), "RESOURCE")
      (",p", po::value<std::string>()->required(), "PORT")

      (",m", po::value<std::string>()->default_value("no"), "METADATA")
      (",t", po::value<size_t>()->default_value(5), "TIMEOUT")

      (",P", po::value<size_t>()->default_value(std::numeric_limits<size_t>::max()), "UDP PORT")
      (",B", po::value<std::string>()->default_value("none"), "MULTICAST ADDR")
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

  auto udp_port = pseudo_opt_to_std_opt(var_map["-P"].as<size_t>(), std::numeric_limits<size_t>::max());
  auto multicast_addr = pseudo_opt_to_std_opt(var_map["-B"].as<std::string>(), std::string("none"));
  size_t client_timeout = var_map["-T"].as<size_t>();

  try {
    static auto main =
        udp_port ? proxy::Main(host, resource, port, want_meta, timeout, *udp_port, multicast_addr, client_timeout)
                 : proxy::Main(host, resource, port, want_meta, timeout);
    main.main();
  } catch (const exceptions::RadioException &err) {
    std::cerr << err.what() << '\n';
    exit(1);
  }
}
