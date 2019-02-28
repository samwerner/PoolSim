#include "block_event.h"

namespace poolsim {

void to_json(nlohmann::json& j, const BlockEvent& data) {
    j = nlohmann::json{
        {"pool_name", data.pool_name},
        {"miner_address", data.miner_address},
        {"reward_scheme_data", data.reward_scheme_data}
    };
}

}
