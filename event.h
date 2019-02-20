#ifndef EVENT_H
#define EVENT_H

class Event_Queue;

class Event {
 protected:
  double time;

 public:
  Event();

  Event(double _time);
  
  virtual ~Events();

  double get_time() const;

  virtual int execute(Event_Queue* = nullptr) = 0;
  
};


#endif
