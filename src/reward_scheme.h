#pragma once

#include <memory>

#include <nlohmann/json.hpp>

#include "share.h"
#include "factory.h"
#include "miner_record.h"

class MiningPool;

class RewardScheme {
public:
    virtual ~RewardScheme();

    virtual void handle_share(const std::string& miner_address, const Share& share) = 0;

    // Set the mining pool for this reward scheme
    // RewardScheme and MiningPool should be a 1 to 1 relationship
    void set_mining_pool(std::shared_ptr<MiningPool> mining_pool);

    // Returns the mining_pool of this reward scheme as a shared_ptr
    // Use this rather than accessing the weak_ptr property
    std::shared_ptr<MiningPool> get_mining_pool();
protected:
    std::weak_ptr<MiningPool> mining_pool;
};

MAKE_FACTORY(RewardSchemeFactory, RewardScheme, const nlohmann::json&);

template <typename T, typename RecordClass=MinerRecord>
class BaseRewardScheme :
    public RewardScheme,
    public Creatable1<RewardScheme, T, const nlohmann::json&> {
protected:
    std::vector<std::shared_ptr<RecordClass>> records;

    // increments mined block and credits stats for a given record
    virtual void update_record(std::shared_ptr<RecordClass> record, const Share& share) = 0;

    std::shared_ptr<RecordClass> find_record(const std::string miner_address);
};

template <typename T, typename RecordClass>
std::shared_ptr<RecordClass> BaseRewardScheme<T, RecordClass>::find_record(const std::string miner_address) {
  for (auto iter = records.begin(); iter != records.end(); ++iter) {
    if ((*iter)->get_miner() == miner_address)
      return (*iter);
  }

  auto record = std::make_shared<RecordClass>(miner_address);
  records.push_back(record);
  return record;
}

// Pay-per-share reward scheme
class PPSRewardScheme: public BaseRewardScheme<PPSRewardScheme> {
public:
    explicit PPSRewardScheme(const nlohmann::json& args);

    void handle_share(const std::string& miner_address, const Share& share) override;

private:
    void update_record(std::shared_ptr<MinerRecord> record, const Share& share) override;
};


// Pay-per-last-n-shares reward scheme
class PPLNSRewardScheme: public BaseRewardScheme<PPLNSRewardScheme> {
public:
    explicit PPLNSRewardScheme(const nlohmann::json& args);

    void handle_share(const std::string& miner_address, const Share& share) override;

    void set_n(uint64_t _n);

private:
    void update_record(std::shared_ptr<MinerRecord> record, const Share& share) override;

    // the number of last shares over which a reward will be distributed
    uint64_t n;

};

// Queue-based reward scheme
class QBRewardScheme: public BaseRewardScheme<QBRewardScheme, QBRecord> {
public:
    explicit QBRewardScheme(const nlohmann::json& args);

    void handle_share(const std::string& miner_address, const Share& share) override;
    
    uint64_t get_credits(const std::string& miner_address);

protected:
    // updates stats of top miner in pool and resets the top miners credits
    void reward_top_miner();
    void update_record(std::shared_ptr<QBRecord> record, const Share& share) override;

};

