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

}
