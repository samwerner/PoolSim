#pragma once

#include "network_param_handler.h"

namespace poolsim {

class BTCParamHandler : NetworkParamHandler {
    private:
        ~BTCParamHandler();

        void  update_diff() override;

        void update_hash_rate() override; 
};

}