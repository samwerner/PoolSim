#include "default_share_handler.h"

void DefaultShareHandler::handle_share(MiningPool& pool, const Share& share) {
  pool.submit_share(miner->get_address(), share);
}
