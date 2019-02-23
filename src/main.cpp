#include <iostream>
#include <string>
#include <boost/program_options.hpp>

#include <spdlog/spdlog.h>

#include "simulator.h"

#include "miner_creator.h"

namespace po = boost::program_options;


int main(int argc, char* argv[]) {
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "shows help")
      ("config-file", po::value<std::string>(), "path to config file")
      ("debug", "enable debug logs")
      ("seed", po::value<long>(), "random seed to use")
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

  spdlog::set_level(spdlog::level::info);
  if (vm.count("debug")) {
    spdlog::set_level(spdlog::level::debug);
  }

  long seed = 0;
  if (vm.count("seed")) {
    seed = vm["seed"].as<long>();
  }

  std::string config_filepath = vm["config-file"].as<std::string>();

  SystemRandom::initialize(seed);
  spdlog::debug("initialized random with seed {}", seed);

  auto simulation = Simulation::from_config_file(config_filepath);
  Simulator simulator(simulation);

  simulator.run();

  return 0;
}
