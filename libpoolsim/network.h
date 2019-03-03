#pragma once

#include <memory>
#include <vector>
#include <cstdint>


namespace poolsim {

class MiningPool;

class Network {

public:
    explicit Network(uint64_t difficulty);
    void register_pool(std::shared_ptr<MiningPool> pool);
    std::vector<std::shared_ptr<MiningPool>> get_pools();
    uint64_t get_difficulty() const;
private:
    uint64_t difficulty;
    void set_difficulty(uint64_t difficulty);
    std::vector<std::shared_ptr<MiningPool>> pools;
};

}
