#pragma once

#include <string>
#include <memory>

#include "share.h"
#include "mining_pool.h"
#include "share_handler.h"
#include "network.h"

namespace poolsim {

class Miner : public std::enable_shared_from_this<Miner> {
public:
    // Miners can only be created through this method
    // as we only want to create them as shared_ptr
    // for the ShareHandler to be able to reference them
    static std::shared_ptr<Miner> create(
        std::string _address, double _hashrate,
        std::unique_ptr<ShareHandler> share_handler,
        std::shared_ptr<Network> network
        );
    virtual ~Miner() {}

    std::string get_address() const;
    double get_hashrate() const;
    std::shared_ptr<MiningPool> get_pool() const;

    void set_handler(std::unique_ptr<ShareHandler> handler);

    // Processes the share by delegating to different strategies
    virtual void process_share(const Share& share);

    // Joins the given pool, updates the state of the pool too
    void join_pool(std::shared_ptr<MiningPool> pool);

    // Returns the network instance
    std::shared_ptr<Network> get_network() const;


protected:
    Miner(std::string _address, double _hashrate, std::shared_ptr<Network> network);

private:
    std::string address;
    double hashrate;
    std::weak_ptr<MiningPool> pool;

    std::unique_ptr<ShareHandler> share_handler;
    std::weak_ptr<Network> network;
};

}
