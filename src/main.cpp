#include <iostream>
#include <string>

#include <CLI11.hpp>
#include <spdlog/spdlog.h>

#include "simulator.h"

#include "miner_creator.h"


int main(int argc, char* argv[]) {
    CLI::App app{"PoolSim: Extensible Mining pool simulator"};

    std::string config_filepath;
    bool debug = false;
    long seed = 0;

    app.add_option("-c,--config-file", config_filepath, "configuration file")
        ->required()
        ->check(CLI::ExistingFile);
    app.add_flag("--debug", debug, "enable debug logs");
    app.add_option("--seed", seed, "random seed to use");

    CLI11_PARSE(app, argc, argv);

    spdlog::set_level(spdlog::level::info);
    if (debug) {
        spdlog::set_level(spdlog::level::debug);
    }

    SystemRandom::initialize(seed);
    spdlog::debug("initialized random with seed {}", seed);

    auto simulation = Simulation::from_config_file(config_filepath);
    auto simulator = std::make_shared<Simulator>(simulation);

    simulator->run();

    return 0;
}
