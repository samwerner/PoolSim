#pragma once

#include <string>
#include <memory>
#include <cstdint>
#include <nlohmann/json.hpp>

class MinerRecord {
public:
    MinerRecord(std::string _address);

    // increments balance of blocks mined by miner
    void inc_blocks_mined();
    // increments balance of total blocks received by miner
    void inc_blocks_received();
    // increments total number of blocks received by miner by specified amount
    void inc_blocks_received(double _block);
    // increments total balance of uncles mined by miner
    void inc_uncles_mined();
    // increments total balance of uncles received by miner
    void inc_uncles_received();
    // increments total balance of uncles received by miner by specified amount
    void inc_uncles_received(double _uncles);
    // returns address of miner to which record belongs
    std::string get_miner() const;
    // returns the number of shares submitted
    uint64_t get_shares_count() const;
    //
    uint64_t get_uncles_mined() const;
    //
    uint64_t get_uncles_received() const;
    //
    uint64_t get_blocks_mined() const;
    //
    uint64_t get_blocks_received() const;

    // increment the shares count
    void inc_shares_count();
protected:
    uint64_t blocks_mined = 0, uncles_mined = 0, shares_count = 0; 
    
    double blocks_received = 0, uncles_received = 0;

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
    // writes miner record data to json object
    void to_json(nlohmann::json& j);
    // updates the average credits a miner had when he was rewarded a block
    void update_avg_credits_per_block();

private:
    uint64_t credits = 0, avg_credits_per_block = 0;
};

class QBSortObj {
public:
    inline bool operator()(std::shared_ptr<QBRecord> left_record, std::shared_ptr<QBRecord> right_record) {
        return (left_record->get_credits() > right_record->get_credits());
    }
};

void to_json(nlohmann::json& j, const MinerRecord& data);