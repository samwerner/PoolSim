#pragma once

#include <nlohmann/json.hpp>

#include "mining_pool.h"
#include "share.h"
#include "factory.h"

namespace poolsim {

class Miner;

class ShareHandler {
public:
  virtual ~ShareHandler();

  // Miners delegates to this method to handle the share that it found
  // TODO: Probaby add information about the environment here
  // e.g. info about other mining pools, current network difficulty, etc
  virtual void handle_share(const Share& share) = 0;

  // Set the miner for this share handler
  // ShareHandler and Miner should must a 1 to 1 relationship
  void set_miner(std::shared_ptr<Miner> miner);

  // Returns the miner of this handler as a shared_ptr
  // Use this rather than accessing the weak_ptr property
  std::shared_ptr<Miner> get_miner();
protected:
  std::weak_ptr<Miner> miner;
};

MAKE_FACTORY(ShareHandlerFactory, ShareHandler, const nlohmann::json&)


template <typename T>
class BaseShareHandler :
  public ShareHandler,
  public Creatable1<ShareHandler, T, const nlohmann::json&> {
};


// Default implementation for ShareHandler
class DefaultShareHandler: public BaseShareHandler<DefaultShareHandler> {
public:
  explicit DefaultShareHandler(const nlohmann::json& args);
  // Simply submits the share to the mining pool
  virtual void handle_share(const Share& share) override;
};

}
