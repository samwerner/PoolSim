#include "share.h"

namespace poolsim {

Share::Share(uint8_t _properties): properties(_properties) {}


uint8_t Share::get_properties() const { return properties; }
bool Share::is_valid_block() const { return (properties & Property::valid_block) != 0; }
bool Share::is_uncle() const { return (properties & Property::uncle) != 0; }
bool Share::is_network_share() const { return is_valid_block() && !is_uncle(); }


bool operator==(const Share& lhs, const Share& rhs) {
  return lhs.get_properties() == rhs.get_properties();
}

bool operator!=(const Share& lhs, const Share& rhs) {
  return !(lhs == rhs);
}

}
