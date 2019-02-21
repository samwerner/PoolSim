#include <iostream>
#include <string>
#include <boost/program_options.hpp>


#include "simulator.h"

namespace po = boost::program_options;


int main(int argc, char* argv[]) {
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "shows help")
      ("config-file", po::value<std::string>(), "path to config file")
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
      std::cerr << desc << std::endl;
      return 1;
  }

  if (!vm.count("config-file")) {
    std::cerr << "--config-file not given" << std::endl;
    return 1;
  }

  std::string config_filepath = vm["config-file"].as<std::string>();

  auto simulation = Simulation::from_config_file(config_filepath);
  Simulator simulator(simulation);

  simulator.run();


  return 0;
}
