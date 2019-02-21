#pragma once

#include <string>

class MiningPool;

class Miner {
 private:
  std::string address;
  double hashrate;
  MiningPool *pool;
  unsigned long long credits, shares;
  unsigned long blocks_mined, blocks_received, uncles_mined, uncles_received;
  
 public:
  Miner(std::string _address, double _hashrate, MiningPool *_pool);

  // schedules next mining share event in eventqueue
  // bool schedule_share(EventQueue *queue);

  // miner submits network share to pool
  void network_share();

  //miner submits normal share to pool
  void pool_share();
  
  // sets credit balance of miner to _credits
  void set_credits(unsigned long long _credits);

  // increments balance of blocks mined by miner
  void inc_blocks_mined();

  // increments balance of total blocks received by mined
  void inc_blocks_received();

  // increments total balance of uncles mined by miner
  void inc_uncles_mined();

  // increments total balance of uncles received by miner
  void inc_uncles_received();

};
