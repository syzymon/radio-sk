#include <boost/program_options.hpp>
#include "client/main.h"

namespace po = boost::program_options;
/*
 * Parametry:

-H host – adres, na którym nasłuchuje radio-proxy, lub adres rozgłoszeniowy, lub adres rozsyłania grupowego, obowiązkowy;

-P port – numer portu UDP, na którym nasłuchuje radio-proxy, liczba dziesiętna, obowiązkowy;

-p port – numer portu TCP, na który można podłączyć się programem telnet w celu sterowania klientem, obowiązkowy;

-T timeout– czas w sekundach, po którego upłynięciu, jeśli pośrednik przestał przysyłać dane, klient uznaje, że
            pośrednik przestał działać, domyślna wartość 5 sekund, opcjonalny.
 */
int main(int argc, char *argv[]) {
  po::options_description desc("parameters");
  desc.add_options()
      (",H", po::value<std::string>()->required(), "RADIO-PROXY SERVER HOST")
      (",P", po::value<uint16_t>()->required(), "RADIO-PROXY SERVER PORT")
      (",p", po::value<uint16_t>()->required(), "TCP TELNET INTERFACE PORT")
      (",T", po::value<size_t>()->default_value(5), "SECS TIMEOUT");

  po::variables_map var_map;
  try {
    po::store(po::parse_command_line(argc, argv, desc), var_map);
    po::notify(var_map);
  } catch (po::error &err) {
    std::cerr << err.what() << std::endl;
    exit(1);
  }

  auto proxy_host = var_map["-H"].as<std::string>();
  auto proxy_port = var_map["-P"].as<uint16_t>();
  auto client_port = var_map["-P"].as<uint16_t>();
  auto secs_timeout = var_map["-T"].as<size_t>();

  static auto main = client::Main(proxy_host, proxy_port, client_port, secs_timeout);
  main.main();
}