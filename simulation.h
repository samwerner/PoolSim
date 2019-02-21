
#include <fstream>

struct Simulation {
  static Simulation from_config_file(const std::string& filepath);

  uint64_t rounds;
};

