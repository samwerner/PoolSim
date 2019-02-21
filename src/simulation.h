#pragma once

#include <iostream>

struct Simulation {
  // Creates a Simulation from a config file
  static Simulation from_config_file(const std::string& filepath);
  // Creates a Simulation from a stream
  static Simulation from_stream(std::istream& stream);
  // Creates a Simulation from a JSON string
  static Simulation from_string(const std::string& string);

  uint64_t rounds;
};

