#include "event.h"

namespace poolsim {

Event::Event(const std::string& _miner_address, double _time):
  miner_address(_miner_address), time(_time) {}

}
