#pragma once

#include "miner.h"

class Reward_Scheme {
 public:
  Reward_Scheme();

  virtual void network_share(Miner *miner) = 0;

  virtual void pool_share(Miner *miner) = 0;
};
