#include <fstream>
#include <iostream>

#include "simulator.h"
#include "json.hpp"


Simulator::Simulator(Simulation _simulation) : simulation(_simulation) {}

Simulator Simulator::from_config_file(const std::string& filepath) {
  auto simulation = Simulation::from_config_file(filepath);
  return Simulator(simulation);
}


void Simulator::run() {
  std::cout << "running " << simulation.rounds << " rounds" << std::endl;
}
