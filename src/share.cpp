#include "share.h"

Share::Share(bool _network_share): network_share(_network_share) {}

bool Share::is_network_share() const { return network_share; }


bool operator==(const Share& lhs, const Share& rhs) {
  return lhs.is_network_share() == rhs.is_network_share();
}

bool operator!=(const Share& lhs, const Share& rhs) {
  return !(lhs == rhs);
}