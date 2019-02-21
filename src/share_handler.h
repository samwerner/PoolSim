#pragma once

#include "mining_pool.h"
#include "share.h"

class Miner;

class ShareHandler {
public:
  ShareHandler(std::shared_ptr<Miner> _miner): miner(_miner) {}

  // Miners delegats to this method to handle the share that it found
  // TODO: Probaby add information about the environment here
  // e.g. info about other mining pools, current network difficulty, etc
  virtual void handle_share(const Share& share) = 0;

protected:
  std::shared_ptr<Miner> miner;
};
