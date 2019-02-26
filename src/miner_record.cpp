#include "miner_record.h"
#include <string>

MinerRecord::MinerRecord(std::string miner_address) : address(miner_address) {}

std::string MinerRecord::get_miner() const {
    return address;
}

void MinerRecord::inc_blocks_mined() {
    blocks_mined++;
}

void MinerRecord::inc_blocks_received() {
    blocks_received++;
}

void MinerRecord::inc_uncles_mined() {
    uncles_mined++;
}

void MinerRecord::inc_uncles_received() {
    uncles_received++;
}

uint64_t MinerRecord::get_shares_count() const {
    return shares_count;
}

void MinerRecord::inc_shares_count() {
    shares_count++;
}

void MinerRecord::to_json(nlohmann::json& j) {
    j = nlohmann::json{{"miner_address", address}, {"shares_count", shares_count}, {"uncles_mined", uncles_mined},
        {"uncles_received", uncles_received}, {"blocks_mined", blocks_mined}, {"blocks_received", blocks_received}};
}


QBRecord::QBRecord(std::string miner_address) : MinerRecord(miner_address) {}

void QBRecord::set_credits(uint64_t balance) {
    credits = balance;
}

uint64_t QBRecord::get_credits() const {
   return credits;
}

void QBRecord::inc_credits(uint64_t _credits) {
    credits += _credits;
}

void QBRecord::update_avg_credits_per_block() {
    if (blocks_received)
        avg_credits_per_block = (avg_credits_per_block + credits)/blocks_received;
}

void QBRecord::to_json(nlohmann::json& j) {
     j = nlohmann::json{{"miner_address", address}, {"shares_count", shares_count}, {"uncles_mined", uncles_mined},
        {"uncles_received", uncles_received}, {"blocks_mined", blocks_mined}, {"blocks_received", blocks_received},
        {"avg_credits_per_received_block", avg_credits_per_block}};
}
