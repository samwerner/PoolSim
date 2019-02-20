#include "Compare.h"
#include "Event.h"

bool Compare::operator()(const Event *l, const Event *r) {
    return l->get_time() > r->get_time();
}
