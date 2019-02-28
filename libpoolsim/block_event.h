#pragma once

#include <string>
#include "nlohmann/json.hpp"


namespace poolsim {

struct BlockEvent {
    std::string pool_name;
    std::string miner_address;
    nlohmann::json reward_scheme_data;

};

}
