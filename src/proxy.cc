#include <iostream>
#include <boost/program_options.hpp>

namespace opts = boost::program_options;

int main(int argc, char *argv[]) {
  opts::options_description desc("parameters");
  desc.add_options()
      (",h", opts::value<std::string>()->required(), "RADIO SERVER HOST")
      (",r", opts::value<std::string>()->required(), "RESOURCE")
      (",p", opts::value<uint16_t>()->required(), "PORT")
      // TODO: handle only yes below
      (",m", opts::value<std::string>()->default_value("no"), "METADATA")
      (",t", opts::value<size_t>()->default_value(5), "TIMEOUT")
      // TODO: part B args
      ;

  opts::variables_map var_map;
  try {
    opts::store(opts::parse_command_line(argc, argv, desc), var_map);
    opts::notify(var_map);
  } catch (opts::error &err) {
    std::cerr << err.what() << std::endl;
    exit(1);
  }
}
