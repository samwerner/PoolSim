#include "qb_sort_obj.h"

bool QBSortObj::operator()(std::shared_ptr<QBRecord> left_record, std::shared_ptr<QBRecord> right_record) {
    return (left_record->get_credits() > right_record->get_credits());
}