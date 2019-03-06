#pragma once

#include <nlohmann/json.hpp>

#include "mining_pool.h"
#include "share.h"
#include "factory.h"

namespace poolsim {

class Miner;

struct BehaviourConfig {
    uint64_t top_n = 0;
    double threshold = 0;
};

struct MultiAddressConfig : BehaviourConfig {
    uint64_t addresses = 0;
};

struct QBHoppingConfig : BehaviourConfig {
    double bad_luck_limit = 1;
};


class ShareHandler {
public:
    virtual ~ShareHandler();

    // Miners delegates to this method to handle the share that it found
    virtual void handle_share(const Share& share) = 0;

    // Returns the name of the share handler
    virtual std::string get_name() const = 0;

    // Returns the metadata of the share handler (if any)
    virtual nlohmann::json get_json_metadata() = 0;

    // Set the miner for this share handler
    // ShareHandler and Miner must be a 1 to 1 relationship
    void set_miner(std::shared_ptr<Miner> miner);

    // Returns the miner of this handler as a shared_ptr
    // Use this rather than accessing the weak_ptr property
    const std::shared_ptr<Miner> get_miner() const;

    // Returns the network
    const std::shared_ptr<Network> get_network() const;

    // Returns the pool
    std::shared_ptr<MiningPool> get_pool() const;


    // Returns the address
    std::string get_address() const;
protected:
    std::weak_ptr<Miner> miner;

    // random instance
    std::shared_ptr<Random> random = SystemRandom::get_instance();
};

MAKE_FACTORY(ShareHandlerFactory, ShareHandler, const nlohmann::json&)

class QBShareHandler : public ShareHandler {
public:
    virtual void handle_share(const Share& share) = 0;

    nlohmann::json get_json_metadata() override;

    uint64_t get_shares_donated() const;

    uint64_t get_shares_withheld() const;

    uint64_t get_valid_shares_donated() const;
    
protected:
    // Checks if the current mining pool uses a queue based reward scheme
    bool is_pool_queue_based() const;
    // Checks the specified condition logic under which a share should
    // be submitted
    bool should_attack(std::vector<std::shared_ptr<QBRecord>>& records);
    // returns the address of the attack victim
    std::string get_victim_address(std::vector<std::shared_ptr<QBRecord>>& records);
    // used to check if miner is in top N of the pool
    uint64_t top_n = 0;
    // used to check if credits of another miner are within a specified range
    double threshold = 1;
    // counts total number of shares withheld
    uint64_t shares_withheld = 0;
    // counts total number of shares donated to some other address
    uint64_t shares_donated = 0;
    // counts total number of valid shares donated
    uint64_t valid_shares_donated = 0;
};

template <typename T>
class BaseShareHandler :
    public ShareHandler,
    public Creatable1<ShareHandler, T, const nlohmann::json&> {   
public:
    nlohmann::json get_json_metadata() override;
};

template <typename T>
nlohmann::json BaseShareHandler<T>::get_json_metadata() {
    return nlohmann::json::object();
}


template <typename T>
class QBBaseShareHandler :
    public QBShareHandler,
    public Creatable1<ShareHandler, T, const nlohmann::json&> {
};
  
// Default implementation for ShareHandler
class DefaultShareHandler: public BaseShareHandler<DefaultShareHandler> {
public:
    explicit DefaultShareHandler(const nlohmann::json& args);
    // Simply submits the share to the mining pool
    virtual void handle_share(const Share& share) override;

    std::string get_name() const override;
};

// IMPLEMENTED: YES
// Behaviour: if the share is valid, the miner does not submit the share to the pool. This is 
// a traditional block withholding attack.
// Else, the share is submitted as specified by the default behaviour. 
class WithholdingShareHandler : public BaseShareHandler<WithholdingShareHandler> {
public:
    explicit WithholdingShareHandler(const nlohmann::json& args);
    // Withholds valid shares (including uncles) from submitting to pool operator
    void handle_share(const Share& share) override;

    std::string get_name() const override;
};

// IMPLEMENTED: YES
// Behaviour: miner checks if he is currently in the top N positions in the queue of the pool
// and whether there is a miner with a credit balance of p% or less behind him. If this is true,
// the miner submits his share to a different address of the N addresses he controls in the pool.
// Else, the share is submitted as specified by the default behaviour.
class QBWithholdingShareHandler : public QBBaseShareHandler<QBWithholdingShareHandler> {
//class QBWithholdingShareHandler : public BaseShareHandler<QBWithholdingShareHandler, QBRewardScheme> {
public:
    explicit QBWithholdingShareHandler(const nlohmann::json& args);
    // Withholds valid shares (including uncles) from submitting to pool operator
    void handle_share(const Share& share) override;

    std::string get_name() const override;
private:
    // counts the total number of valid shares withheld
    uint64_t valid_shares_withheld = 0;
};

// IMPLEMENTED: YES
// Behaviour: miner checks if he is currently in the top N positions in the queue of the pool
// and whether there is a miner with a credit balance of p% or less behind him. If this is true,
// the miner submits his share to a different address of the N addresses he controls in the pool.
// Else, the share is submitted as specified by the default behaviour.
class DonationShareHandler : public QBBaseShareHandler<DonationShareHandler> {
public:
    explicit DonationShareHandler(const nlohmann::json& args);
    // Donates the share to a specified address if a defined condition is true
    void handle_share(const Share& share) override;

    std::string get_name() const override;
};

// IMPLEMENTED: YES
// Behaviour: miner checks if he is currently in the top N positions in the queue of the pool
// and whether there is a miner with a credit balance of p% or less behind him. If this is true,
// the miner submits his share to a different address of the N addresses he controls in the pool.
// Else, the share is submitted as specified by the default behaviour.
class MultipleAddressesShareHandler : public QBBaseShareHandler<MultipleAddressesShareHandler> {
public:
    explicit MultipleAddressesShareHandler(const nlohmann::json& args);
    // Donates the share to any of the other addresses belonging to
    // the miner in a pool
    void handle_share(const Share& share) override;

    uint64_t get_addresses_count() const;

    std::string get_name() const override;
private:
    // list of all addresses in pool controlled by miner
    std::vector<std::string> addresses;
    // returns an address owned by the miner at random from the list of addresses
    std::string get_random_address() const;
};


struct HopEvent {
    std::string previous_pool;
    std::string next_pool;
    uint64_t time;
};

// IMPLEMENTED: YES
// Behaviour: miner defines a bad luck limit and checks if the current pool is as unlucky or worse.
// If it is, the miner checks the current luck for all other pools in the network and joins the pool
// that is luckiest. 
class QBPoolHopping : public QBBaseShareHandler<QBPoolHopping> {
public:
    explicit QBPoolHopping(const nlohmann::json& args);

    void handle_share(const Share& share) override;

    std::string get_name() const override;

    nlohmann::json get_json_metadata() override;
private:
    std::shared_ptr<MiningPool> get_luckiest_pool();
    // the factor of how unlucky the pool needs to be before leaving the pool (e.g. bad luck = 2 = 50% luck)
    double bad_luck_limit = 1;

    // list of hop events
    std::vector<HopEvent> hop_events;
}; 

void to_json(nlohmann::json& j, const HopEvent& data);

void from_json(const nlohmann::json& j, BehaviourConfig& b);
void from_json(const nlohmann::json& j, MultiAddressConfig& m);
void from_json(const nlohmann::json& j, QBHoppingConfig& m);

}
