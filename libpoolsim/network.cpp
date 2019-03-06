#include "network.h"

#include "mining_pool.h"

namespace poolsim {

Network::Network(uint64_t _difficulty) : difficulty(_difficulty) {}

void Network::register_pool(std::shared_ptr<MiningPool> pool) {
    pools.push_back(pool);
}

std::vector<std::shared_ptr<MiningPool>> Network::get_pools() {
    return pools;
}

uint64_t Network::get_difficulty() const { return difficulty; }

void Network::set_difficulty(uint64_t _difficulty) { difficulty = _difficulty; }

uint64_t Network::get_current_time() const { return current_time; }
void Network::set_current_time(uint64_t _current_time) { current_time = _current_time; }

uint64_t Network::get_current_block() const { return current_block; }
void Network::inc_current_block() { current_block++; }

}
