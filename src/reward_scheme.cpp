#include "reward_scheme.h"
#include "miner.h"
#include "factory.h"

#include "mining_pool.h"
#include "miner_record.h"
#include <algorithm>

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


PPLNSRewardScheme::PPLNSRewardScheme(const nlohmann::json& _args) {}

void PPLNSRewardScheme::handle_share(const std::string& miner_address, const Share& share) {
    // TODO: do some realy cool stuff with this share
    auto record = find_record(miner_address);
    update_record(record, share);
    
    if (share.is_network_share()) {

    } else if (share.is_uncle()) {

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
    } else {
    std::sort(records.begin(), records.end(), QBSortObj());
    records[0]->inc_blocks_received();
    records[0]->set_credits(records[0]->get_credits() - records[1]->get_credits());
    //TODO: log proportion of credits lost per round
    }
  }
}

void QBRewardScheme::handle_share(const std::string& miner_address, const Share& share) {
  auto record = this->find_record(miner_address);
  this->update_record(record, share);
  if (share.is_network_share())
    this->reward_top_miner();
  else if (share.is_uncle()) {
    // TODO: decide on uncle reward scheme
    
  }
}

uint64_t QBRewardScheme::get_credits(const std::string& miner_address) {
    auto record = this->find_record(miner_address);
    return record->get_credits();
}


REGISTER(RewardScheme, QBRewardScheme, "qb")
