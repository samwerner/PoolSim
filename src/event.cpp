#include "event.h"

Event::Event(double _time) : time(_time) {}

double Event::get_time() const {
  return time;
}
