#include <algorithm>
#include <cassert>

#include "reward_scheme.h"
#include "miner.h"
#include "factory.h"
#include "random.h"

#include "mining_pool.h"
#include "miner_record.h"

namespace poolsim {

void from_json(const nlohmann::json& j, PPLNSConfig& r) {
    j.at("n").get_to(r.n);
    if (j.find("pool_fee") != j.end())
        j.at("pool_fee").get_to(r.pool_fee);
}
void from_json(const nlohmann::json& j, RewardConfig& r) {

    if (j.find("pool_fee") != j.end())
        j.at("pool_fee").get_to(r.pool_fee);
}

void to_json(nlohmann::json& j, const BlockMetaData& b) {
    j = nlohmann::json{
        {"shares_per_block", b.shares_per_block},
        {"pool_luck", b.pool_luck}
    };
}

void to_json(nlohmann::json& j, const QBBlockMetaData& b) {
    j = nlohmann::json{
        {"shares_per_block", b.shares_per_block},
        {"pool_luck", b.pool_luck},
        {"credit_balance_receiver", b.credit_balance_receiver},
        {"receiver_address", b.receiver_address},
        {"reset_balance_receiver", b.reset_balance_receiver},
        {"proportion_credits_lost", b.prop_credits_lost}
    };
}

RewardScheme::~RewardScheme() {}

void RewardScheme::set_pool_fee(double _fee) {
    assert(_fee >= 0 && _fee <= 1);
    pool_fee = _fee;
} 

void RewardScheme::set_mining_pool(std::shared_ptr<MiningPool> _mining_pool) {
  mining_pool = _mining_pool;
}

std::shared_ptr<MiningPool> RewardScheme::get_mining_pool() {
  return mining_pool.lock();
}

double RewardScheme::get_pool_luck() {
    if (shares_per_block == 0) {
        return 0.0;
    }
    auto pool = get_mining_pool();
    uint64_t network_difficulty = get_mining_pool()->get_network()->get_difficulty();
    double exp_shares_per_block = (double)network_difficulty/pool->get_difficulty();
    double pool_luck = (exp_shares_per_block / shares_per_block) * 100.0; 

    return pool_luck;
}

std::string PPSRewardScheme::get_scheme_name() const {
    return "PPS";
}

PPSRewardScheme::PPSRewardScheme(const nlohmann::json& _args) {
    RewardConfig pps_config;
    from_json(_args, pps_config);
    set_pool_fee(pps_config.pool_fee);
}

void PPSRewardScheme::handle_share(const std::string& miner_address, const Share& share) {
   shares_per_block++;
   auto record = find_record(miner_address);
   update_record(record, share);

    if (!share.is_valid_block())
        return;
    
    block_meta_data.shares_per_block = shares_per_block;
    block_meta_data.pool_luck = get_pool_luck();

    if (!share.is_uncle()) {
        record->inc_blocks_mined();
        shares_per_block = 0;
        return;
    }

    handle_uncle(miner_address);
    record->inc_uncles_mined();
}

void PPSRewardScheme::handle_uncle(const std::string& miner_address) {
    // Not relevant for a traditional PPS scheme, as all shares are paid for directly by the pool
}

void PPSRewardScheme::update_record(std::shared_ptr<MinerRecord> record, const Share& share) {
    record->inc_shares_count();
    uint64_t network_difficulty = get_mining_pool()->get_network()->get_difficulty();
    double p = get_mining_pool()->get_difficulty()/(double)network_difficulty;
    record->inc_blocks_received((1-pool_fee)*p);
}

REGISTER(RewardScheme, PPSRewardScheme, "pps")


std::string PPLNSRewardScheme::get_scheme_name() const {
    return "PPLNS";
}

PPLNSRewardScheme::PPLNSRewardScheme(const nlohmann::json& _args) {
    PPLNSConfig pplns_config;
    from_json(_args, pplns_config);
    set_n(pplns_config.n);
    set_pool_fee(pplns_config.pool_fee);
}

void PPLNSRewardScheme::insert_share(std::string miner_address) {
    last_n_shares.push_back(miner_address);
    while (last_n_shares.size() > n) {
        last_n_shares.pop_front();
    }
}

void PPLNSRewardScheme::handle_share(const std::string& miner_address, const Share& share) {
    auto miner_record = find_record(miner_address);
    update_record(miner_record, share);
    shares_per_block++;
    insert_share(miner_address);

    if (!share.is_valid_block())
        return;

    block_meta_data.shares_per_block = shares_per_block;
    block_meta_data.pool_luck = get_pool_luck();

    if (!share.is_uncle()) {
        for (const std::string& miner_address : last_n_shares) {
            auto record = find_record(miner_address);
            record->inc_blocks_received((1.0/last_n_shares.size()));
        }
        shares_per_block = 0;
        return;
    }

    handle_uncle(miner_address);
}

void PPLNSRewardScheme::update_record(std::shared_ptr<MinerRecord> record, const Share& share) {
    record->inc_shares_count();
    if (share.is_network_share())
        record->inc_blocks_mined();
    else if (share.is_uncle())
        record->inc_uncles_mined();
}

void PPLNSRewardScheme::set_n(uint64_t _n) {
    assert(_n > 0);
    n = _n;
}

void PPLNSRewardScheme::handle_uncle(const std::string& miner_address) {
    for (const std::string& miner_address : last_n_shares) {
        auto record = find_record(miner_address);
        record->inc_uncles_received(1.0/last_n_shares.size());
    }
}

REGISTER(RewardScheme, PPLNSRewardScheme, "pplns")

std::string QBRewardScheme::get_scheme_name() const {
    return "QB";
}

QBRewardScheme::QBRewardScheme(const nlohmann::json& _args) {
    RewardConfig qb_config;
    from_json(_args, qb_config);
    set_pool_fee(qb_config.pool_fee);
}

void QBRewardScheme::update_record(std::shared_ptr<QBRecord> record, const Share& share) {
    auto share_difficulty = get_mining_pool()->get_difficulty();
    record->inc_credits(share_difficulty);
    record->inc_shares_count();
    if (share.is_network_share()) {
        record->inc_blocks_mined();
        record->update_avg_credits_per_block();
    } else if (share.is_uncle())
        record->inc_uncles_mined();
}

void QBRewardScheme::reward_top_miner() {
    if (records.empty()) {
        return;
    }

    if (records.size() == 1) {
        records[0]->inc_blocks_received();
        block_meta_data.credit_balance_receiver = records[0]->get_credits();
        block_meta_data.receiver_address = records[0]->get_miner();
        shares_per_block = 0;
        return;
    }

    std::sort(records.begin(), records.end(), QBSortObj());
    records[0]->inc_blocks_received();
    block_meta_data.credit_balance_receiver = records[0]->get_credits();
    block_meta_data.receiver_address = records[0]->get_miner();
    uint64_t credits_diff = records[0]->get_credits() - records[1]->get_credits();
    block_meta_data.reset_balance_receiver = credits_diff;

    uint64_t credits_sum = get_credits_sum();
    if (credits_sum)
        block_meta_data.prop_credits_lost = (double)records[1]->get_credits()/credits_sum;
    else
        block_meta_data.prop_credits_lost = 0;

    shares_per_block = 0;
    records[0]->set_credits(credits_diff);
}

uint_fast64_t QBRewardScheme::get_credits_sum() {
    uint64_t sum = 0;
    for (auto record : records) {
        sum += record->get_credits();
    }
    return sum;
}

void QBRewardScheme::handle_share(const std::string& miner_address, const Share& share) {
    shares_per_block++;
    auto record = this->find_record(miner_address);
    this->update_record(record, share);
    
    if (!share.is_valid_block())
        return;
    
    block_meta_data.shares_per_block = shares_per_block;
    block_meta_data.pool_luck = get_pool_luck();

    if (share.is_network_share()) {
        this->reward_top_miner();
    } else if (share.is_uncle()) {
        handle_uncle(miner_address);
    }
}

uint64_t QBRewardScheme::get_credits(const std::string& miner_address) {
    auto record = this->find_record(miner_address);
    return record->get_credits();
}

void QBRewardScheme::handle_uncle(const std::string& miner_address) {
    auto random_miner = random->random_element(records.begin(), records.end());
    random_miner->inc_uncles_received();
}

REGISTER(RewardScheme, QBRewardScheme, "qb")

std::string PROPRewardScheme::get_scheme_name() const {
    return "PROP";
}

PROPRewardScheme::PROPRewardScheme(const nlohmann::json& _args) {
    RewardConfig prop_config;
    from_json(_args, prop_config);
    set_pool_fee(prop_config.pool_fee);
}

void PROPRewardScheme::handle_share(const std::string& miner_address, const Share& share) {
    shares_per_block++;
    auto record = find_record(miner_address);
    update_record(record, share);

    if (!share.is_valid_block())
        return;

    block_meta_data.shares_per_block = shares_per_block;
    block_meta_data.pool_luck = get_pool_luck();

    if (share.is_network_share()) {
        for (auto miner_record : records) {
            double reward = 1.0*(miner_record->get_shares_per_round()/(double)shares_per_block);
            miner_record->inc_blocks_received(reward);
            miner_record->reset_shares_per_round();
        }
        shares_per_block = 0;
    } else if (share.is_uncle()) {
        handle_uncle(miner_address);
    }
}

void PROPRewardScheme::handle_uncle(const std::string& miner_address) {
    for (auto record : records) {
        double reward = (record->get_shares_per_round()/(double)shares_per_block);
        record->inc_uncles_received(reward);
    }
}

std::list<std::string>& PPLNSRewardScheme::get_last_n_shares() {
    return last_n_shares;
}

uint64_t PPLNSRewardScheme::get_last_n_shares_size() const {
    return last_n_shares.size();
}

void PROPRewardScheme::update_record(std::shared_ptr<MinerRecord> record, const Share& share) {
    record->inc_shares_count();
    record->inc_shares_per_round();

    if (!share.is_valid_block())
        return;

    if (share.is_uncle()) {
        record->inc_uncles_mined();
        return;
    }

    record->inc_blocks_mined();
}

REGISTER(RewardScheme, PROPRewardScheme, "prop")

}

