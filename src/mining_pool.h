#pragma once

#include <set>
#include <string>
#include <memory>
#include <cstdint>

#include "reward_scheme.h"
#include "share.h"
#include "random.h"


class MiningPool : public std::enable_shared_from_this<MiningPool> {
public:
  static std::shared_ptr<MiningPool> create(
    uint64_t difficulty, double uncle_prob, std::unique_ptr<RewardScheme> reward_scheme);

  static std::shared_ptr<MiningPool> create(
    uint64_t difficulty, double uncle_prob,
    std::unique_ptr<RewardScheme> reward_scheme, std::shared_ptr<Random> random);

  // Returns all the miners currently in the pool
  std::set<std::string> get_miners();

  // Returns the current number of miners
  size_t get_miners_count() const;

  // Returns the share difficulty of the pool
  uint64_t get_difficulty() const;

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

protected:
  MiningPool(uint64_t difficulty, double uncle_prob, std::shared_ptr<Random> random);

private:
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

  // Random instance
  std::shared_ptr<Random> random;
};
