#pragma once

class Event {
protected:
  double time;

public:
  Event(double _time);
  
  double get_time() const;
};
