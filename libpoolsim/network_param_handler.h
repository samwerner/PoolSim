#pragma once

namespace poolsim {

class NetworkParamHandler {
    public:
        void update_params();

    protected:
        std::shared_ptr<Network> network;

        virtual ~NetworkParamHandler();

        virtual void update_diff() = 0;

        virtual void update_hash_rate() = 0;
};       

}