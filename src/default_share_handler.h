#pragma once

#include "share_handler.h"

// Default implementation for ShareHandler
class DefaultShareHandler: public ShareHandler {
public:
  // Simply submits the share to the mining pool
  virtual void handle_share(const Share& share) override;
};
