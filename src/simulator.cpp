#include <fstream>
#include <iostream>

#include <spdlog/spdlog.h>

#include "simulator.h"


Simulator::Simulator(Simulation _simulation) : simulation(_simulation) {}

Simulator Simulator::from_config_file(const std::string& filepath) {
  auto simulation = Simulation::from_config_file(filepath);
  return Simulator(simulation);
}


void Simulator::run() {
  spdlog::info("running {} rounds", simulation.rounds);
}
