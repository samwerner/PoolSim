#include <iostream>
#include <string>

#include <CLI11.hpp>
#include <spdlog/spdlog.h>

#include "cli.h"
#include "simulator.h"
#include "miner_creator.h"


Cli::Cli() {
    app = std::make_shared<CLI::App>("PoolSim: Extensible Mining pool simulator");
    args = std::make_shared<CliArgs>();
    app->add_option("-c,--config-file", args->config_filepath, "configuration file")
       ->required()
       ->check(CLI::ExistingFile);
    app->add_flag("--debug", args->debug, "enable debug logs");
}

int Cli::run(int argc, char* argv[]) {
    app->parse(argc, argv);

    spdlog::set_level(spdlog::level::info);
    if (args->debug) {
        spdlog::set_level(spdlog::level::debug);
    }

    auto simulator = Simulator::from_config_file(args->config_filepath);
    simulator->run();

    return 0;
}

std::shared_ptr<CLI::App> Cli::get_app() {
    return app;
}
