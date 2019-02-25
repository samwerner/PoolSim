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

