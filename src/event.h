#pragma once

#include <string>

struct Event {
  Event(const std::string& miner_address, double _time);

  std::string miner_address;
  double time;
};

class CompareEvents {
public:
  inline bool operator()(const Event& l, const Event& r) {
    return l.time > r.time;
  }
};
