#include "btc_param_handler.h"
#include "network.h"

namespace poolsim {

void BTCParamHandler::update_diff() {
    // check 2016 block counter
    if (network->blocks.size() % 2016 != 0)
        return;

    // get duration for 2016 blocks
    uint64_t start = network->blocks.size - 2016;
    double actual_time = network->blocks.back().timestamp - network->blocks[start].timestamp;
    
    // compute new diff based on exp. time (seconds for 2016 blocks in BTC)
    const double expected_time = 1209600;
    if (actual_time == expected_time)
        return;

    double adjustment_factor = expected_time/actual_time;
    if (adjustment_factor > 4) {
        adjustment_factor = 4;
    } else if (adjustment_factor < 0.25) {
        adjustment_factor = 0.25;
    }

    network->difficulty *= adjustment_factor;

    if (network->difficulty < 1)
        network->difficulty = 1;
}

void BTCParamHandler::update_hash_rate() {
    // TODO!
    // currently remain constant
} 

}