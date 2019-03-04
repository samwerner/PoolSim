#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <list>
#include <map>

#include "share.h"
#include "factory.h"
#include "miner_record.h"

namespace poolsim {

class MiningPool;

struct RewardConfig {
    double pool_fee = 0;
};

struct PPLNSConfig : RewardConfig {
    uint64_t n = 0;
};

struct BlockMetaData {
    uint64_t shares_per_block = 0;
    double pool_luck = 0;
};

struct QBBlockMetaData : BlockMetaData {
    uint64_t credit_balance_receiver = 0;
    std::string receiver_address;
    uint64_t reset_balance_receiver = 0;
    double prop_credits_lost = 0;
};

class RewardScheme {
public:
    virtual ~RewardScheme();

    virtual void handle_share(const std::string& miner_address, const Share& share) = 0;

    // Set the mining pool for this reward scheme
    // RewardScheme and MiningPool should be a 1 to 1 relationship
    void set_mining_pool(std::shared_ptr<MiningPool> mining_pool);

    // returns the metadata of the last block mined (including uncle blocks)
    virtual nlohmann::json get_block_metadata() = 0;

    // returns the metadata for a miner
    virtual nlohmann::json get_miner_metadata(const std::string& miner_address) = 0;

    // returns the name of the reward scheme
    virtual std::string get_scheme_name() const = 0;

    // Returns the mining_pool of this reward scheme as a shared_ptr
    // Use this rather than accessing the weak_ptr property
    std::shared_ptr<MiningPool> get_mining_pool();
    // sets the pool operator fee (expressed as a perecentage)
    void set_pool_fee(double _fee);

    // returns the luck of the mining pool for the current round
    double get_pool_luck();

    // USED FOR TESTING
    virtual double get_blocks_received(const std::string& miner_address) = 0;
    virtual uint64_t get_blocks_mined(const std::string& miner_address) = 0;
    virtual std::shared_ptr<MinerRecord> get_record(const std::string& miner_address) = 0;

protected:
    // logic for distributing uncle block reward in pool
    virtual void handle_uncle(const std::string& miner_address) = 0;
    
    std::weak_ptr<MiningPool> mining_pool;
    // number of shares submitted per block mined (NOT including uncles)
    uint64_t shares_per_block = 0;
    // the percentage of a block reward taken by the pool operator
    double pool_fee = 0;    
};

MAKE_FACTORY(RewardSchemeFactory, RewardScheme, const nlohmann::json&);

template <typename T, typename RecordClass=MinerRecord, typename BlockData=BlockMetaData>
class BaseRewardScheme :
    public RewardScheme,
    public Creatable1<RewardScheme, T, const nlohmann::json&> {
protected:
    std::vector<std::shared_ptr<RecordClass>> records;

    // increments mined block and credits stats for a given record
    virtual void update_record(std::shared_ptr<RecordClass> record, const Share& share) = 0;

    // returns the metadata needed when a block has been mined
    virtual nlohmann::json get_block_metadata() override;

    // returns the metadata for a miner
    virtual nlohmann::json get_miner_metadata(const std::string& miner_address) override;

    // returns record of a miner if it exists, otherwise a new record is created and returned
    std::shared_ptr<RecordClass> find_record(const std::string& miner_address);

    //stores the meta data associated to the last block mined
    BlockData block_meta_data;
    
    // USED FOR TESTING
    virtual double get_blocks_received(const std::string& miner_address) override;
    virtual uint64_t get_blocks_mined(const std::string& miner_address) override;
    virtual std::shared_ptr<MinerRecord> get_record(const std::string& miner_address) override;
};

template <typename T, typename RecordClass, typename BlockData>
std::shared_ptr<RecordClass> BaseRewardScheme<T, RecordClass, BlockData>::find_record(const std::string& miner_address) {
  for (auto iter = records.begin(); iter != records.end(); ++iter) {
    if ((*iter)->get_miner() == miner_address)
      return (*iter);
  }

  auto record = std::make_shared<RecordClass>(miner_address);
  records.push_back(record);
  return record;
}

template<typename T, typename RecordClass, typename BlockData>
nlohmann::json BaseRewardScheme<T, RecordClass, BlockData>::get_block_metadata() {
    nlohmann::json j;
    to_json(j, block_meta_data);
    return j;
}

template<typename T, typename RecordClass, typename BlockData>
nlohmann::json BaseRewardScheme<T, RecordClass, BlockData>::get_miner_metadata(const std::string& miner_address) {
    nlohmann::json j;
    to_json(j, *find_record(miner_address));
    return j;
}

// USED FOR TESTING
template<typename T, typename RecordClass, typename BlockData>
double BaseRewardScheme<T, RecordClass, BlockData>::get_blocks_received(const std::string& miner_address) {
    auto record = find_record(miner_address);
    return record->get_blocks_received();
}

// USED FOR TESTING
template<typename T, typename RecordClass, typename BlockData>
uint64_t BaseRewardScheme<T, RecordClass, BlockData>::get_blocks_mined(const std::string& miner_address) {
    auto record = find_record(miner_address);
    return record->get_blocks_mined();
}

// USED FOR TESTING
template<typename T, typename RecordClass, typename BlockData>
std::shared_ptr<MinerRecord> BaseRewardScheme<T, RecordClass, BlockData>::get_record(const std::string& miner_address) {
    auto record = find_record(miner_address);
    return record;
}

// Pay-per-share reward scheme
class PPSRewardScheme: public BaseRewardScheme<PPSRewardScheme> {
public:
    explicit PPSRewardScheme(const nlohmann::json& args);

    std::string get_scheme_name() const override;

    void handle_share(const std::string& miner_address, const Share& share) override;
private:
    void handle_uncle(const std::string& miner_address) override;

    void update_record(std::shared_ptr<MinerRecord> record, const Share& share) override;
};

// Pay-per-last-n-shares reward scheme
class PPLNSRewardScheme: public BaseRewardScheme<PPLNSRewardScheme> {
public:
    explicit PPLNSRewardScheme(const nlohmann::json& args);

    std::string get_scheme_name() const override;

    void handle_share(const std::string& miner_address, const Share& share) override;

    void set_n(uint64_t _n);
    // a flag for showing whether a total of n or more shares have been been submitted
    bool n_shares_submitted = false;

    // USED FOR TESTS
    std::list<std::string>& get_last_n_shares();
    uint64_t get_last_n_shares_size() const;

private:
    void handle_uncle(const std::string& miner_address) override;
    
    void update_record(std::shared_ptr<MinerRecord> record, const Share& share) override;

    // the number of last shares over which a reward will be distributed
    uint64_t n = 0;
    // list of miner addresses that submitted last n shares
    std::list<std::string> last_n_shares;
    // inserts a miners address to the list of address of last n miners that submitted a share
    void insert_share(std::string miner_address);
};

// Queue-based reward scheme
class QBRewardScheme: public BaseRewardScheme<QBRewardScheme, QBRecord, QBBlockMetaData> {
public:
    explicit QBRewardScheme(const nlohmann::json& args);

    std::string get_scheme_name() const override;

    void handle_share(const std::string& miner_address, const Share& share) override;
    
    uint64_t get_credits(const std::string& miner_address);

protected:
    // by default: sample a random miner from the pool for receiving the full uncle reward
    void handle_uncle(const std::string& miner_address) override;
    // updates stats of top miner in pool and resets the top miners credits
    void reward_top_miner();
    // updates the given record based on the type of share accordingly 
    void update_record(std::shared_ptr<QBRecord> record, const Share& share) override;
    // returns the total sum of credit balances by pool
    uint64_t get_credits_sum();
};

// Proportional reward scheme
class PROPRewardScheme: public BaseRewardScheme<PROPRewardScheme> {
public:
    explicit PROPRewardScheme(const nlohmann::json& args);

    std::string get_scheme_name() const override;

    void handle_share(const std::string& miner_address, const Share& share) override;

private:
    void handle_uncle(const std::string& miner_address) override;   
    
    void update_record(std::shared_ptr<MinerRecord> record, const Share& share) override;
};

void from_json(const nlohmann::json& j, PPLNSConfig& r);
void from_json(const nlohmann::json& j, RewardConfig& r);
void to_json(nlohmann::json& j, const BlockMetaData& b);
void to_json(nlohmann::json& j, const QBBlockMetaData& b);

}
