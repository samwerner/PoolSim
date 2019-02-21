#include <fstream>
#include <iostream>

#include <easyloggingpp/easylogging++.h>

#include "simulator.h"


Simulator::Simulator(Simulation _simulation) : simulation(_simulation) {}

Simulator Simulator::from_config_file(const std::string& filepath) {
  auto simulation = Simulation::from_config_file(filepath);
  return Simulator(simulation);
}


void Simulator::run() {
  LOG(INFO) << "running " << simulation.rounds << " rounds";
}
