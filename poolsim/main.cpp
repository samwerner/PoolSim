#include <iostream>
#include <string>

#include <CLI11.hpp>
#include <spdlog/spdlog.h>

#include "simulator.h"
#include "miner_creator.h"
#include "cli.h"


int main(int argc, char* argv[]) {
    Cli cli;

    try {
        cli.run(argc, argv);
    } catch(const CLI::ParseError &e) {
        return cli.get_app()->exit(e);
    }

    return 0;
}
