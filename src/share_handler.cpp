#include "share_handler.h"
#include "miner.h"
#include "factory.h"


ShareHandler::~ShareHandler() {}

void ShareHandler::set_miner(std::shared_ptr<Miner> _miner) {
  miner = _miner;
}

std::shared_ptr<Miner> ShareHandler::get_miner() {
  return miner.lock();
}

// NOTE: this particular class probably does not need for args
// but it must accepts them because of the current factory implementation
DefaultShareHandler::DefaultShareHandler(const nlohmann::json& _args) {}

void DefaultShareHandler::handle_share(const Share& share) {
  get_miner()->get_pool()->submit_share(get_miner()->get_address(), share);
}

REGISTER(ShareHandler, DefaultShareHandler, "default")
