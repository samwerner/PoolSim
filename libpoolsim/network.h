#pragma once

#include <memory>
#include <vector>
#include <cstdint>
#include <utility>
#include "network_param_handler.h"

namespace poolsim {

class MiningPool;
class Simulator;

struct Block {
    uint64_t number;
    double timestamp;
    uint64_t difficulty;
};

class Network {
friend class Simulator;
friend class BTCParamHandler;
public:
    explicit Network(uint64_t difficulty);
    void register_pool(std::shared_ptr<MiningPool> pool);
    std::vector<std::shared_ptr<MiningPool>> get_pools();
    uint64_t get_difficulty() const;
    uint64_t get_current_time() const;
    uint64_t get_current_block() const;
    uint64_t get_hash_rate() const;
    std::string get_name() const;
    void add_block();
private:
    uint64_t total_hash_rate = 0;
    std::vector<Block> blocks;
    std::string name = "default";
    uint64_t difficulty;
    uint64_t current_time = 0;
    uint64_t current_block = 0;
    void inc_current_block();
    void set_difficulty(uint64_t difficulty);
    std::vector<std::shared_ptr<MiningPool>> pools;
    void set_current_time(uint64_t time);
};

}
