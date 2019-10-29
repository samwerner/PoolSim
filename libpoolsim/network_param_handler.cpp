#include "network_param_handler.h"

namespace poolsim {

void NetworkParamHandler::update_params() {
    update_diff();
    update_hash_rate();
}

NetworkParamHandler::~NetworkParamHandler() {}

}