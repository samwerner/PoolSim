#include "share_handler.h"
#include "miner.h"
#include "factory.h"
#include "miner_record.h"
#include "random.h"

#include <spdlog/spdlog.h>

#include <iterator>
#include <algorithm>

namespace poolsim {

void from_json(const nlohmann::json& j, BehaviourConfig& b) {
    if (j.find("top_n") != j.end())
        j.at("top_n").get_to(b.top_n);
    if (j.find("threshold") != j.end())
        j.at("threshold").get_to(b.threshold);
}

void from_json(const nlohmann::json& j, MultiAddressConfig& m){
    if (j.find("top_n") != j.end())
        j.at("top_n").get_to(m.top_n);
    if (j.find("threshold") != j.end())
        j.at("threshold").get_to(m.threshold);
    if (j.find("addresses") != j.end())
        j.at("addresses").get_to(m.addresses);
}

void from_json(const nlohmann::json& j, QBHoppingConfig& m){
    if (j.find("top_n") != j.end())
        j.at("top_n").get_to(m.top_n);
    if (j.find("threshold") != j.end())
        j.at("threshold").get_to(m.threshold);
    if (j.find("bad_luck_limit") != j.end())
        j.at("bad_luck_limit").get_to(m.bad_luck_limit);
}

ShareHandler::~ShareHandler() {}

void ShareHandler::set_miner(std::shared_ptr<Miner> _miner) {
  miner = _miner;
}

const std::shared_ptr<Miner> ShareHandler::get_miner() const {
  return miner.lock();
}

// NOTE: this particular class probably does not need for args
// but it must accept them because of the current factory implementation
DefaultShareHandler::DefaultShareHandler(const nlohmann::json& _args) {}

void DefaultShareHandler::handle_share(const Share& share) {
  get_miner()->get_pool()->submit_share(get_miner()->get_address(), share);
}

std::string DefaultShareHandler::get_name() const {
    return "default";
}

REGISTER(ShareHandler, DefaultShareHandler, "default")

WithholdingShareHandler::WithholdingShareHandler(const nlohmann::json& _args) {}

void WithholdingShareHandler::handle_share(const Share& share) {
    if (!share.is_valid_block())
        get_miner()->get_pool()->submit_share(get_miner()->get_address(), share);
    
}

uint64_t ShareHandler::get_valid_shares_withheld() const{
    return valid_shares_withheld;
}

std::string WithholdingShareHandler::get_name() const {
    return "share_withholding";
}

REGISTER(ShareHandler, WithholdingShareHandler, "share_withholding")

uint64_t QBShareHandler::get_shares_donated() const {
    return shares_donated;
}

uint64_t QBShareHandler::get_shares_withheld() const {
    return shares_withheld;
}

uint64_t QBShareHandler::get_valid_shares_donated() const {
    return valid_shares_donated;
}

bool QBShareHandler::should_attack(std::vector<std::shared_ptr<QBRecord>>& records) {
    return get_victim_address(records) != "";
}

bool QBShareHandler::is_pool_queue_based() const {
    return get_miner()->get_pool()->get_scheme_name() == "QB";
}

std::string QBShareHandler::get_victim_address(std::vector<std::shared_ptr<QBRecord>>& records) {
    for (size_t i = 0; i + 1 < records.size() && i < top_n; i++) {
        auto record = records[i];
        if (record->get_miner_address() == get_miner()->get_address()) {
            auto victim = records[i + 1];
            if (record->get_credits() * threshold <= victim->get_credits()) {
                return victim->get_miner_address();
            } 
        }
    }
    return "";
}

QBWithholdingShareHandler::QBWithholdingShareHandler(const nlohmann::json& _args) {
    BehaviourConfig config;
    from_json(_args, config);
    top_n = config.top_n;
    threshold = config.threshold;
}

void QBWithholdingShareHandler::handle_share(const Share& share) {
    if (!is_pool_queue_based()) {
        get_miner()->get_pool()->submit_share(get_miner()->get_address(), share);
        return;   
    }
    
    auto records = get_miner()->get_pool()->get_records<QBRewardScheme>();
    std::sort(records.begin(), records.end(), QBSortObj());
    
    if (!should_attack(records)) {
        get_miner()->get_pool()->submit_share(get_miner()->get_address(), share);
        return;
    }

    shares_withheld++;
    if (share.is_valid_block())
        valid_shares_withheld++;
}

std::string QBWithholdingShareHandler::get_name() const {
    return "qb_share_withholding";
}

REGISTER(ShareHandler, QBWithholdingShareHandler, "qb_share_withholding")


DonationShareHandler::DonationShareHandler(const nlohmann::json& _args) {
    BehaviourConfig config;
    from_json(_args, config);
    top_n = config.top_n;
    threshold = config.threshold;
}

void DonationShareHandler::handle_share(const Share& share) {
    if (!is_pool_queue_based()) {
        get_miner()->get_pool()->submit_share(get_miner()->get_address(), share);
        return;   
    }
    
    auto records = get_miner()->get_pool()->get_records<QBRewardScheme>();
    std::sort(records.begin(), records.end(), QBSortObj());
    
    std::string victim_address = get_victim_address(records);
    if (victim_address == "") {
        get_miner()->get_pool()->submit_share(get_miner()->get_address(), share);
        return;
    }

    shares_donated++;
    if (share.is_valid_block())
        valid_shares_donated++;

    get_miner()->get_pool()->submit_share(victim_address, share);
}

std::string DonationShareHandler::get_name() const {
    return "share_donation";
}

REGISTER(ShareHandler, DonationShareHandler, "share_donation")

MultipleAddressesShareHandler::MultipleAddressesShareHandler(const nlohmann::json& _args) {
    MultiAddressConfig config;
    from_json(_args, config);
    top_n = config.top_n;
    threshold = config.threshold;
    uint64_t addresses_count = config.addresses;
    if (addresses_count == 0) {
        throw std::invalid_argument("'addresses' must be set when using multiple_addresses");
    }

    for (size_t count = 0; count < addresses_count; count++) {
        std::string new_address = random->get_address();
        addresses.push_back(new_address);
    }
}

uint64_t MultipleAddressesShareHandler::get_addresses_count() const {
    return addresses.size();
}

std::string MultipleAddressesShareHandler::get_random_address() const {
    return random->random_element(addresses.begin(), addresses.end());
}

void MultipleAddressesShareHandler::handle_share(const Share& share) {
    if (!is_pool_queue_based()) {
        get_miner()->get_pool()->submit_share(get_miner()->get_address(), share);
        return;   
    }
    
    auto records = get_miner()->get_pool()->get_records<QBRewardScheme>();
    std::sort(records.begin(), records.end(), QBSortObj());
    
    if (!should_attack(records)) {
        get_miner()->get_pool()->submit_share(get_miner()->get_address(), share);
        return;
    }

    shares_donated++;
    if (share.is_valid_block())
        valid_shares_donated++;


    std::string other_address = get_random_address();
    // NOTE: join will be a no-op if the other address is already in the pool
    get_miner()->get_pool()->join(other_address);
    get_miner()->get_pool()->submit_share(other_address, share);
}

std::string MultipleAddressesShareHandler::get_name() const {
    return "multiple_addresses";
}

REGISTER(ShareHandler, MultipleAddressesShareHandler, "multiple_addresses");


QBPoolHopping::QBPoolHopping(const nlohmann::json& _args) {
    QBHoppingConfig config;
    from_json(_args, config);
    top_n = config.top_n;
    threshold = config.threshold;
    bad_luck_limit = config.bad_luck_limit;
}

void QBPoolHopping::handle_share(const Share& share) {
    if (!is_pool_queue_based()) {
        get_miner()->get_pool()->submit_share(get_miner()->get_address(), share);
        return;
    }

    auto records = get_miner()->get_pool()->get_records<QBRewardScheme>();
    std::sort(records.begin(), records.end(), QBSortObj());

    /*
    if (!should_attack(records)) {
        get_miner()->get_pool()->submit_share(get_miner()->get_address(), share);
        return;
    }
    */

    // Leave pool if pool is unlucky
    if (get_miner()->get_pool()->get_luck() < 100.0/bad_luck_limit) {
        
        // Check which other pool is the luckiest 
        std::shared_ptr<MiningPool> luckiest_pool = get_luckiest_pool();
        if (luckiest_pool != get_miner()->get_pool()) {
            get_miner()->join_pool(luckiest_pool);
            total_hopps++;
       }
    }

    get_miner()->get_pool()->submit_share(get_miner()->get_address(), share);
}

std::shared_ptr<MiningPool> QBPoolHopping::get_luckiest_pool() {
    std::vector<std::shared_ptr<MiningPool>> pools = get_miner()->get_network()->get_pools();
    double pool_luck = get_miner()->get_pool()->get_luck();
    std::shared_ptr<MiningPool> luckiest_pool = get_miner()->get_pool();
    for (auto pool : pools) {
        if (pool->get_luck() > pool_luck && pool != luckiest_pool) {
            pool_luck = pool->get_luck();
            luckiest_pool = pool;
        }
    }
    return luckiest_pool;
}

std::string QBPoolHopping::get_name() const {
    return "qb_pool_hopping";
}

REGISTER(ShareHandler, QBPoolHopping, "qb_pool_hopping");



}
