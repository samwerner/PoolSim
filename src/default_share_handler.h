#pragma once

#include "share_handler.h"

class DefaultShareHandler: public ShareHandler {
public:
  virtual void handle_share(MiningPool& pool, const Share& share) override;
};
