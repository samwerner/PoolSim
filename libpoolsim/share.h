#pragma once

#include <cstdint>



class Share {
public:
  enum Property {
    none = 0,
    network = 1 << 0,
    uncle = 1 << 1,
  };

  Share(uint8_t _properties);

  uint8_t get_properties() const;
  bool is_network_share() const;
  bool is_uncle() const;
private:
  // Stores properties about the share
  // bit 0 is rightmost bit
  // bit 0: is_network
  // bit 1: is_uncle
  uint8_t properties;
};

bool operator==(const Share& lhs, const Share& rhs);
bool operator!=(const Share& lhs, const Share& rhs);
