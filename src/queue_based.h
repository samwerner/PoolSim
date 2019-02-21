#ifndef QUEUEBASED_H
#define QUEUEBASED_H

class Queue_Based : public Reward_Scheme {
 private:
  
  
 public:
  Queue_Based();

  void pool_share(Miner *miner);

  void network_share(Miner *miner);
  
}

#endif


