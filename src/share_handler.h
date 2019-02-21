#pragma once

#include "miner.h"
#include "mining_pool.h"
#include "share.h"

class ShareHandler {
public:
  ShareHandler(std::shared_ptr<Miner> _miner): miner(_miner) {}

  virtual void handle_share(
    MiningPool& pool,
    const Share& share
    // TODO: Probaby add information about the environment here
    // e.g. info about other mining pools, current network difficulty, etc
  ) = 0;

protected:
  std::shared_ptr<Miner> miner;
};
