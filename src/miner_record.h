#pragma once

#include <string>
#include <cstdint>

class MinerRecord {
public:
    MinerRecord(std::string _address);

    // increments balance of blocks mined by miner
    void inc_blocks_mined();
    // increments balance of total blocks received by mined
    void inc_blocks_received();
    // increments total balance of uncles mined by miner
    void inc_uncles_mined();
    // increments total balance of uncles received by miner
    void inc_uncles_received();
    // returns address of miner to which record belongs
    std::string get_miner() const;
private:
    uint64_t blocks_mined, blocks_received, uncles_mined, uncles_received;
    std::string address;
};

class QBRecord : public MinerRecord {
public:
    QBRecord(std::string miner_address);
    // increments credits by amount '_credits'
    void inc_credits(uint64_t _credits);
    // sets credits of a miner to function argument 'balance'
    void set_credits(uint64_t balance);
    // returns credits of a miner
    uint64_t get_credits() const;
    
private:
    uint64_t credits;
};
