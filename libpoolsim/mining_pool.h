#pragma once

#include <set>
#include <string>
#include <memory>
#include <cstdint>

#include "reward_scheme.h"
#include "network.h"
#include "share.h"
#include "random.h"
#include "observer.h"
#include "block_event.h"


namespace poolsim {

class MiningPool : public std::enable_shared_from_this<MiningPool>,
                   public Observable<BlockEvent> {
public:
    static std::shared_ptr<MiningPool> create(
        const std::string& name,
        uint64_t difficulty,
        double uncle_prob,
        std::unique_ptr<RewardScheme> reward_scheme,
        std::shared_ptr<Network> network);

    static std::shared_ptr<MiningPool> create(
        const std::string& name,
        uint64_t difficulty,
        double uncle_prob,
        std::unique_ptr<RewardScheme> reward_scheme,
        std::shared_ptr<Network> network,
        std::shared_ptr<Random> random);

    // Returns all the miners currently in the pool
    std::set<std::string> get_miners();

    // Returns the name of the reward scheme used by the pool
    std::string get_scheme_name() const;

    // Returns the current number of miners
    size_t get_miners_count() const;

    // Returns the share difficulty of the pool
    uint64_t get_difficulty() const;

    // Returns the name of the pool
    std::string get_name() const;

    // Returns the network instance
    std::shared_ptr<Network> get_network() const;

    // Submit a share to the pool
    // The share can be either a network share or a pool share
    // TODO: when the share is a network share this should probably return
    // if it became an uncle block or not
    void submit_share(const std::string& miner_address, const Share& share);

    // Joins this mining pool
    // This method does not update the miner state
    void join(const std::string& miner);

    // Leaves this mining pool
    // This method does not update the miner state
    void leave(const std::string& miner_address);

    // Set the reward scheme for this mining pool
    void set_reward_scheme(std::unique_ptr<RewardScheme> _reward_scheme);

    // Returns the metadata of all miners in the poool
    nlohmann::json get_miners_metadata() const;

    //std::vector<std::shared_ptr<MinerRecord>> get_miner_records();

protected:
    MiningPool(const std::string& name, uint64_t difficulty,
               double uncle_prob,
               std::shared_ptr<Network> network,
               std::shared_ptr<Random> random);

private:
    // name of pool
    std::string pool_name;
    // list of miners in pool
    std::set<std::string> miners;
    // share and network difficulty; total hashrate of pool
    uint64_t difficulty;
    // probability that a block is an uncle
    double uncle_prob;
    // reward scheme used by pool for distributing block rewards among miners
    std::unique_ptr<RewardScheme> reward_scheme;
    // total blocks mined by miners in pool
    unsigned long blocks_mined;
    // Information about network
    std::weak_ptr<Network> network;
    // Random instance
    std::shared_ptr<Random> random;
};

void to_json(nlohmann::json& j, const MiningPool& data);

}
