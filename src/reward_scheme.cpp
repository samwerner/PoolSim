#include "reward_scheme.h"
#include "miner.h"
#include "factory.h"

#include "mining_pool.h"
#include "miner_record.h"
#include <algorithm>


void from_json(const nlohmann::json& j, PPLNSConfig& r) {
    j.at("n").get_to(r.n);
}

void to_json(nlohmann::json& j, const BlockMetaData& b) {
    j = nlohmann::json{{"miner_address", b.miner_address}, {"reward_scheme", b.reward_scheme}, {"shares_per_block", b.shares_per_block}};
}
void to_json(nlohmann::json& j, const QBBlockMetaData& b) {
    j = nlohmann::json{{"miner_address", b.miner_address}, {"reward_scheme", b.reward_scheme}, {"shares_per_block", b.shares_per_block}, 
                        {"credit_balance_receiver", b.credit_balance_receiver}, {"receiver_address", b.receiver_address}, {"reset_balance_receiver", 
                        b.reset_balance_receiver}, {"proportion_credits_lost", b.prop_credits_lost}};
}

RewardScheme::~RewardScheme() {}

void RewardScheme::set_mining_pool(std::shared_ptr<MiningPool> _mining_pool) {
  mining_pool = _mining_pool;
}

std::shared_ptr<MiningPool> RewardScheme::get_mining_pool() {
  return mining_pool.lock();
}

PPSRewardScheme::PPSRewardScheme(const nlohmann::json& _args) {}

void PPSRewardScheme::handle_share(const std::string& miner_address, const Share& share) {
    // TODO: do some realy cool stuff with this share
    

}

void PPSRewardScheme::update_record(std::shared_ptr<MinerRecord> record, const Share& share) {
    // TODO: implement logic

}

REGISTER(RewardScheme, PPSRewardScheme, "pps")

PPLNSRewardScheme::PPLNSRewardScheme(const nlohmann::json& _args) {
    PPLNSConfig config;
    from_json(_args, config);
    set_n(config.n);
}

// TODO: add tests for this
void PPLNSRewardScheme::insert_share(std::string miner_address) {
    last_n_shares.push_back(miner_address);
    while (last_n_shares.size() > n) {
        last_n_shares.pop_front();
    }
}

// TODO: add tests for this
void PPLNSRewardScheme::handle_share(const std::string& miner_address, const Share& share) {
    auto miner_record = find_record(miner_address);
    update_record(miner_record, share);
    shares_per_block++;
    if (n) {
        last_n_shares.push_back(miner_address);

        if (share.is_network_share()) {
            for (auto iter = last_n_shares.begin(); iter != last_n_shares.end(); ++iter) {
                auto record = find_record((*iter));
                record->inc_blocks_received(1.0/n);
            }
            block_meta_data.shares_per_block = shares_per_block;
            shares_per_block = 0;
            block_meta_data.miner_address = miner_address;
            block_meta_data.reward_scheme = "PPLNS";
        } else if (share.is_uncle()) {
             for (auto iter = last_n_shares.begin(); iter != last_n_shares.end(); ++iter) {
                auto record = find_record((*iter));
                record->inc_uncles_received(1.0/n);
            }
        }
    }
}

void PPLNSRewardScheme::update_record(std::shared_ptr<MinerRecord> record, const Share& share) {
    record->inc_shares_count();
    if (share.is_network_share())
        record->inc_blocks_mined();
    else if (share.is_uncle())
        record->inc_uncles_mined();
}

void PPLNSRewardScheme::set_n(uint64_t _n) {
    n = _n;
}


REGISTER(RewardScheme, PPLNSRewardScheme, "pplns")


QBRewardScheme::QBRewardScheme(const nlohmann::json& _args) {}

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
    if (!records.empty()) {
        if (records.size() == 1) {
            records[0]->inc_blocks_received();
            block_meta_data.credit_balance_receiver = records[0]->get_credits();
            block_meta_data.receiver_address = records[0]->get_miner();
            shares_per_block = 0;
        } else {
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

            block_meta_data.shares_per_block = shares_per_block;
            shares_per_block = 0;         
            records[0]->set_credits(credits_diff);
        }
    }
}

uint_fast64_t QBRewardScheme::get_credits_sum() {
    uint64_t sum = 0;
    for (auto iter = records.begin(); iter != records.end(); ++iter) {
        sum += (*iter)->get_credits();
    }
    return sum;
}

void QBRewardScheme::handle_share(const std::string& miner_address, const Share& share) {
    auto record = this->find_record(miner_address);
    this->update_record(record, share);
    shares_per_block++;
    if (share.is_network_share()) {
        block_meta_data.miner_address = miner_address;
        block_meta_data.reward_scheme = "QB";
        this->reward_top_miner();
    } else if (share.is_uncle()) {
        // TODO: decide on uncle reward scheme

    }
}

uint64_t QBRewardScheme::get_credits(const std::string& miner_address) {
    auto record = this->find_record(miner_address);
    return record->get_credits();
}


REGISTER(RewardScheme, QBRewardScheme, "qb")
