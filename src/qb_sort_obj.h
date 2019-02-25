#pragma once
#include "miner_record.h"
#include <memory>

class QBSortObj {
public:
    bool operator()(std::shared_ptr<QBRecord> left_miner, std::shared_ptr<QBRecord> right_miner);  
};
