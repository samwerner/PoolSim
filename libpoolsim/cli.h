#pragma once

#include <memory>

#include <CLI11.hpp>

#include "simulator.h"

namespace poolsim {

struct CliArgs {
    std::string config_filepath;
    std::string output_filepath;
    bool debug;
};

class Cli {
public:
    Cli();
    int run(int argc, char* argv[]);

    std::shared_ptr<CLI::App> get_app();
private:
    std::shared_ptr<CLI::App> app;
    std::shared_ptr<CliArgs> args;
};


int run(int argc, char* argv[]);

}
