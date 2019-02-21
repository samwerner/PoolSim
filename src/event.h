#pragma once

#include <string>

struct Event {
  Event(const std::string& miner_address, double _time);

  std::string miner_address;
  double time;
};

// Class used to compare Events in priority_queue
class CompareEvents {
public:
  inline bool operator()(const Event& left, const Event& right) {
    return left.time > right.time;
  }
};
