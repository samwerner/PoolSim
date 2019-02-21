#include "default_share_handler.h"
#include "miner.h"

void DefaultShareHandler::handle_share(const Share& share) {
  miner->get_pool()->submit_share(miner->get_address(), share);
}
