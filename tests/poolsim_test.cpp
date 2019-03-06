#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "miner.h"
#include "network.h"
#include "share.h"
#include "mining_pool.h"
#include "event_queue.h"
#include "simulation.h"
#include "simulator.h"
#include "random.h"
#include "reward_scheme.h"
#include "miner_record.h"
#include <memory>
#include <nlohmann/json.hpp>
#include <vector>
#include <iterator>


using namespace poolsim;


const std::string pplns_simulation_string = R"({
  "output": "results.json",
  "blocks": 5,
  "network_difficulty": 100,
  "pools": [{
    "uncle_block_prob": 0.05,
    "difficulty": 10,
    "reward_scheme": {
        "type": "pplns", "params": { 
            "n": 3
        }
    },
    "miners": [{
      "behavior": {"name": "default", "params": {}},
      "generator": "csv",
      "params": {"path": "miners.csv"}
    }]
  }]
})";

const std::string prop_simulation_string = R"({
  "output": "results.json",
  "blocks": 5,
  "network_difficulty": 100,
  "pools": [{
    "uncle_block_prob": 0.05,
    "difficulty": 10,
    "reward_scheme": {
        "type": "prop", "params": {
            "pool_fee": 0
        }
    },
    "miners": [{
      "behavior": {"name": "default", "params": {}},
      "generator": "csv",
      "params": {"path": "miners.csv"}
    }]
  }]
})";

const std::string qb_simulation_string = R"({
  "output": "results.json",
  "blocks": 5,
  "network_difficulty": 1000,
  "pools": [{
    "uncle_block_prob": 0.05,
    "difficulty": 100,
    "reward_scheme": {
        "type": "qb", "params": {}
    },
    "miners": [{
      "behavior": {"name": "default", "params": {}},
      "generator": "csv",
      "params": {"path": "miners.csv"}
    }]
  }]
})";

const std::string pps_simulation_string = R"({
  "output": "results.json",
  "blocks": 5,
  "network_difficulty": 100,
  "pools": [{
    "uncle_block_prob": 0.05,
    "difficulty": 10,
    "reward_scheme": {
        "type": "pps", "params": { 
            "pool_fee": 0
        }
    },
    "miners": [{
      "behavior": {"name": "default", "params": {}},
      "generator": "csv",
      "params": {"path": "miners.csv"}
    }]
  }]
})";


const std::string simulation_string = R"({
  "output": "results.json",
  "blocks": 5,
  "network_difficulty": 100,
  "pools": [{
    "uncle_block_prob": 0.001,
    "difficulty": 10,
    "reward_scheme": {"type": "pps", "params": {}},
    "miners": [{
      "behavior": {"name": "default", "params": {}},
      "generator": "csv",
      "params": {"path": "miners.csv"}
    }]
  }]
})";

const nlohmann::json random_miners_params = R"({
  "behavior": {"name": "default", "params": {}},
  "hashrate": {
    "distribution": "normal",
    "params": {"mean": 10.0, "stddev": 1.5}
  },
  "stop_condition": {
    "type": "miners_count",
    "params": {"value": 100}
  }
})"_json;



class MockRandom : public Random {
public:
    MOCK_METHOD0(drand48, double());
    MOCK_METHOD0(get_address, std::string());
    MOCK_METHOD2(random_int, int(int, int));
    MOCK_METHOD0(get_random_engine, std::shared_ptr<std::default_random_engine>());
};

class MockMiner : public Miner {
public:
    MockMiner(const std::string& address, double hashrate, std::shared_ptr<Network> network)
        : Miner(address, hashrate, network) {}
    MOCK_METHOD1(process_share, void(const Share& share));
};


class MockShareHandler : public ShareHandler {
public:
    MOCK_METHOD1(handle_share, void(const Share&));
    std::string get_name() const override { return "mock"; }
};


class MockRewardScheme : public RewardScheme {
public:
    MOCK_METHOD2(handle_share, void(const std::string&, const Share&));
    MOCK_METHOD0(get_json_metadata, nlohmann::json());
    MOCK_METHOD1(get_miner_metadata, nlohmann::json (const std::string&));
    MOCK_METHOD1(get_blocks_mined, uint64_t (const std::string&));
    MOCK_METHOD1(get_blocks_received, double (const std::string&));
    MOCK_METHOD1(handle_uncle, void(const std::string& miner_address));
    MOCK_METHOD1(get_record, std::shared_ptr<MinerRecord> (const std::string&));
    std::string get_scheme_name() const override { return "mock"; }
};

Simulation get_sample_simulation() {
    auto simulation_json = nlohmann::json::parse(simulation_string);
    simulation_json["pools"][0]["miners"][0]["generator"] = "random";
    simulation_json["pools"][0]["miners"][0]["params"] = random_miners_params;
    return simulation_json.get<Simulation>();
}

std::shared_ptr<Network> get_sample_network() {
    return std::make_shared<Network>(100);
}

std::shared_ptr<Simulator> get_sample_simulator(std::shared_ptr<Random> random) {
    return std::make_shared<Simulator>(get_sample_simulation(), random);
}

std::shared_ptr<Simulator> get_sample_simulator() {
    return std::make_shared<Simulator>(get_sample_simulation());
}

std::unique_ptr<MockShareHandler> get_mock_share_handler() {
    return std::unique_ptr<MockShareHandler>(new MockShareHandler);
}

std::unique_ptr<MockRewardScheme> get_mock_reward_scheme() {
    return std::unique_ptr<MockRewardScheme>(new MockRewardScheme);
}

::testing::AssertionResult IsBetweenInclusive(int val, int a, int b)
{
    if((val >= a) && (val <= b))
        return ::testing::AssertionSuccess();
    else
        return ::testing::AssertionFailure()
               << val << " is outside the range " << a << " to " << b;
}


// XXX: this should be at te top of the test suite
// because it expects SystemRandom not to be initialized
// and it performs the initialization
// if we someday need to parallelize the test suite we can think
// about how to make this less ugly but for now it should work just fine
TEST(SystemRandom, initialization) {
    ASSERT_THROW(SystemRandom::get_instance(), RandomInitException);
    SystemRandom::initialize(0);
    ASSERT_NO_THROW(SystemRandom::get_instance());
    // NOTE: expected value with seed = 0
    ASSERT_FLOAT_EQ(SystemRandom::get_instance()->drand48(), 0.170828);
}

TEST(SystemRandom, get_address) {
    auto address = SystemRandom::get_instance()->get_address();
    ASSERT_EQ(address.size(), 42);
    ASSERT_THAT(address, testing::MatchesRegex("0x[0-9a-f]{40}"));
}

TEST(Random, random_element) {
    std::vector<std::shared_ptr<MinerRecord>> records;
    for (size_t i = 0; i < 3; i++) {
        std::shared_ptr<MinerRecord> record = std::make_shared<MinerRecord>("SomeMiner_"+std::to_string(1));
        record->inc_blocks_received(i);
        records.push_back(record);
    }

    std::shared_ptr<MinerRecord> s = SystemRandom::get_instance()->random_element(records.begin(), records.end());
    ASSERT_TRUE(IsBetweenInclusive(s->get_blocks_received(),0,2));
    s = SystemRandom::get_instance()->random_element(records.begin(), records.end());
    ASSERT_TRUE(IsBetweenInclusive(s->get_blocks_received(),0,2));
    s = SystemRandom::get_instance()->random_element(records.begin(), records.end());
    ASSERT_TRUE(IsBetweenInclusive(s->get_blocks_received(),0,2));
    records.clear();

    for (size_t i = 0; i < 200; i++) {
        std::shared_ptr<MinerRecord> record = std::make_shared<MinerRecord>("SomeMiner_"+std::to_string(1));
        record->inc_blocks_received(i);
        records.push_back(record);
    }

    s = SystemRandom::get_instance()->random_element(records.begin(), records.end());
    ASSERT_TRUE(IsBetweenInclusive(s->get_blocks_received(),0,200));
    s = SystemRandom::get_instance()->random_element(records.begin(), records.end());
    ASSERT_TRUE(IsBetweenInclusive(s->get_blocks_received(),0,200));
}

TEST(Share, equality) {
    ASSERT_EQ(Share(Share::Property::valid_block), Share(Share::Property::valid_block));
    uint8_t flags = Share::Property::valid_block | Share::Property::uncle;
    ASSERT_EQ(Share(flags), Share(flags));
    ASSERT_NE(Share(Share::Property::valid_block), Share(Share::Property::uncle));
}

TEST(Share, flags) {
    ASSERT_TRUE(Share(Share::Property::valid_block).is_valid_block());
    ASSERT_TRUE(Share(Share::Property::valid_block).is_network_share());
    ASSERT_TRUE(Share(Share::Property::uncle).is_uncle());
    ASSERT_FALSE(Share(Share::Property::uncle).is_network_share());
    uint8_t flags = Share::Property::valid_block | Share::Property::uncle;
    ASSERT_TRUE(Share(flags).is_uncle());
    ASSERT_TRUE(Share(flags).is_valid_block());
    ASSERT_FALSE(Share(flags).is_network_share());
    ASSERT_FALSE(Share(0).is_network_share());
}

TEST(Simulation, from_string) {
    auto simulation = Simulation::from_string(simulation_string);
    ASSERT_EQ(simulation.blocks, 5);
    ASSERT_EQ(simulation.network_difficulty, 100);
    ASSERT_EQ(simulation.pools.size(), 1);
    auto pool = simulation.pools[0];
    ASSERT_EQ(pool.difficulty, 10);
    ASSERT_FLOAT_EQ(pool.uncle_block_prob, 0.001);
    auto miner_config = pool.miners_config[0];
    ASSERT_EQ(miner_config.generator, "csv");
    ASSERT_EQ(miner_config.params["path"], "miners.csv");
}

TEST(Network, setters_getters) {
    auto network = std::make_shared<Network>(1000);
    ASSERT_EQ(network->get_difficulty(), 1000);
    ASSERT_EQ(network->get_pools().size(), 0);
    auto pool1 = MiningPool::create("pool1", 100, 0.001, get_mock_reward_scheme(), network);
    auto pool2 = MiningPool::create("pool2", 100, 0.001, get_mock_reward_scheme(), network);
    network->register_pool(pool1);
    network->register_pool(pool2);
    ASSERT_EQ(pool1->get_network()->get_difficulty(), 1000);
    ASSERT_EQ(network->get_pools().size(), 2);
    ASSERT_EQ(network->get_pools()[0], pool1);
}

TEST(RewardScheme, get_pool_luck) {
    
    auto simulation = Simulation::from_string(qb_simulation_string);
    ASSERT_EQ(simulation.pools.size(), 1);
    auto reward_config = simulation.pools[0].reward_scheme_config;
    ASSERT_EQ(reward_config.scheme_type, "qb");
    auto params = reward_config.params;
   
    nlohmann::json j;
    std::unique_ptr<QBRewardScheme> qb_ptr = std::unique_ptr<QBRewardScheme>(new QBRewardScheme(j));
    auto base_ptr = RewardSchemeFactory::create(reward_config.scheme_type, reward_config.params);
    auto qb = qb_ptr.get();

    auto pool = simulation.pools[0];
    auto network = std::make_shared<Network>(simulation.network_difficulty);
    auto mining_pool = MiningPool::create("pool", pool.difficulty, pool.uncle_block_prob, std::move(qb_ptr), network);

    ASSERT_EQ(mining_pool->get_network()->get_difficulty(), 1000);
    ASSERT_EQ(mining_pool->get_difficulty(), 100);

    ASSERT_FLOAT_EQ(qb->get_pool_luck(), 0.0);
    for (size_t i = 0; i < 5; i++) {
        qb->handle_share("miner_A", Share(Share::Property::none));
    }
    ASSERT_FLOAT_EQ(qb->get_pool_luck(), 200.0);

    for (size_t i = 0; i < 5; i++) {
        qb->handle_share("miner_A", Share(Share::Property::none));
    }
    ASSERT_FLOAT_EQ(qb->get_pool_luck(), 100.0);
    
    for (size_t i = 0; i < 10; i++) {
        qb->handle_share("miner_A", Share(Share::Property::none));
    }
    ASSERT_FLOAT_EQ(qb->get_pool_luck(), 50.0);

    qb->handle_share("miner_A", Share(Share::Property::valid_block));

    ASSERT_FLOAT_EQ(qb->get_pool_luck(), 0.0);

    for (size_t i = 0; i < 5; i++) {
        qb->handle_share("miner_A", Share(Share::Property::none));
    }
    ASSERT_FLOAT_EQ(qb->get_pool_luck(), 200.0);
}

TEST(Miner, getters) {
  auto miner = Miner::create("random_address", 123, get_mock_share_handler(), get_sample_network());
  ASSERT_EQ(miner->get_address(), "random_address");
  ASSERT_EQ(miner->get_hashrate(), 123);
}

TEST(Miner, join_pool) {
    auto miner = Miner::create("random_address", 123, get_mock_share_handler(), get_sample_network());
    auto pool1 = MiningPool::create("pool1", 100, 0.001, get_mock_reward_scheme(), get_sample_network());
    auto pool2 = MiningPool::create("pool2", 100, 0.001, get_mock_reward_scheme(), get_sample_network());
    ASSERT_EQ(miner->get_pool(), nullptr);
    ASSERT_EQ(pool1->get_miners_count(), 0);
    ASSERT_EQ(pool2->get_miners_count(), 0);
    miner->join_pool(pool1);
    ASSERT_EQ(pool1->get_miners_count(), 1);
    ASSERT_EQ(pool2->get_miners_count(), 0);
    ASSERT_EQ(miner->get_pool(), pool1);
    miner->join_pool(pool2);
    ASSERT_EQ(pool1->get_miners_count(), 0);
    ASSERT_EQ(pool2->get_miners_count(), 1);
    ASSERT_EQ(miner->get_pool(), pool2);
}

TEST(Miner, handle_share) {
    auto share_handler = get_mock_share_handler();
    MockShareHandler* share_handler_ptr = share_handler.get();
    auto network = get_sample_network();
    auto miner = Miner::create("random_address", 123, std::move(share_handler), network);
    auto pool = MiningPool::create("pool", 100, 0.001, get_mock_reward_scheme(), get_sample_network());
    miner->join_pool(pool);
    Share share(Share::Property::none);
    EXPECT_CALL(*share_handler_ptr, handle_share(share));
    miner->process_share(share);
    ASSERT_EQ(miner->get_total_work(), 100);
    ASSERT_EQ(miner->get_blocks_found(), 0);

    Share network_share(Share::Property::valid_block);
    EXPECT_CALL(*share_handler_ptr, handle_share(network_share));
    miner->process_share(network_share);
    ASSERT_EQ(miner->get_total_work(), 200);
    ASSERT_EQ(miner->get_blocks_found(), 1);

    Share uncle_share(Share::Property::valid_block | Share::Property::uncle);
    EXPECT_CALL(*share_handler_ptr, handle_share(uncle_share));
    miner->process_share(uncle_share);
    ASSERT_EQ(miner->get_total_work(), 300);
    ASSERT_EQ(miner->get_blocks_found(), 1);
}


TEST(MiningPool, submit_share) {
    auto reward_scheme = get_mock_reward_scheme();
    auto random = std::make_shared<MockRandom>();
    MockRewardScheme* reward_scheme_ptr = reward_scheme.get();

    auto pool = MiningPool::create("pool", 100, 0.3, std::move(reward_scheme), get_sample_network(), random);

    EXPECT_CALL(*reward_scheme_ptr, handle_share("address", Share(Share::Property::none)));
    pool->submit_share("address", Share(Share::Property::none));

    EXPECT_CALL(*random, drand48()).WillOnce(testing::Return(0.5));
    EXPECT_CALL(*reward_scheme_ptr, handle_share("address", Share(Share::Property::valid_block)));
    pool->submit_share("address", Share(Share::Property::valid_block));

    EXPECT_CALL(*random, drand48()).WillOnce(testing::Return(0.2));
    EXPECT_CALL(*reward_scheme_ptr, handle_share("address", Share(Share::Property::valid_block | Share::Property::uncle)));
    pool->submit_share("address", Share(Share::Property::valid_block));
}

TEST(QBRewardScheme, update_record) {
    auto simulation = Simulation::from_string(qb_simulation_string);
    ASSERT_EQ(simulation.pools.size(), 1);
    auto reward_config = simulation.pools[0].reward_scheme_config;
    ASSERT_EQ(reward_config.scheme_type, "qb");
    auto params = reward_config.params;
   
    nlohmann::json j;
    std::unique_ptr<QBRewardScheme> qb_ptr = std::unique_ptr<QBRewardScheme>(new QBRewardScheme(j));
    auto base_ptr = RewardSchemeFactory::create(reward_config.scheme_type, reward_config.params);
    auto qb = qb_ptr.get();
    
    auto pool = simulation.pools[0];
    auto network = std::make_shared<Network>(simulation.network_difficulty);
    auto mining_pool = MiningPool::create("pool", pool.difficulty, pool.uncle_block_prob, std::move(qb_ptr), network);

    qb->handle_share("address_A", Share(Share::Property::none));
    qb->handle_share("address_B", Share(Share::Property::none));
    qb->handle_share("address_C", Share(Share::Property::none));
    qb->handle_share("address_A", Share(Share::Property::none));

    ASSERT_EQ(qb->get_credits("address_A"), 200);
    ASSERT_EQ(qb->get_credits("address_B"), 100);
    ASSERT_EQ(qb->get_credits("address_C"), 100);

    qb->handle_share("address_B", Share(Share::Property::none));
    qb->handle_share("address_C", Share(Share::Property::none));
    qb->handle_share("address_A", Share(Share::Property::none));
    qb->handle_share("address_B", Share(Share::Property::none));
    qb->handle_share("address_A", Share(Share::Property::none));
    qb->handle_share("address_A", Share(Share::Property::none));

    ASSERT_EQ(qb->get_credits("address_A"), 500);
    ASSERT_EQ(qb->get_credits("address_B"), 300);
    ASSERT_EQ(qb->get_credits("address_C"), 200);

    qb->handle_share("address_B", Share(Share::Property::valid_block));
    ASSERT_EQ(qb->get_credits("address_A"), 100);
    ASSERT_EQ(qb->get_credits("address_B"), 400);

    qb->handle_share("address_B", Share(Share::Property::uncle));
    ASSERT_EQ(qb->get_credits("address_B"), 500);
}

TEST(PPLNSRewardScheme, handle_share) {
    auto simulation = Simulation::from_string(pplns_simulation_string);
    ASSERT_EQ(simulation.pools.size(), 1);
    auto reward_config = simulation.pools[0].reward_scheme_config;
    ASSERT_EQ(reward_config.scheme_type, "pplns");
    ASSERT_EQ(reward_config.params["n"], 3);
    auto params = reward_config.params["n"];
    auto pplns_ptr = RewardSchemeFactory::create(reward_config.scheme_type,
                                                        reward_config.params);
    auto pplns = pplns_ptr.get(); 
    
    auto pool = simulation.pools[0];
    auto network = std::make_shared<Network>(simulation.network_difficulty);
    auto mining_pool = MiningPool::create("pool", pool.difficulty, pool.uncle_block_prob, std::move(pplns_ptr), network);
    pplns->handle_share("miner_D", Share(Share::Property::valid_block));
    ASSERT_EQ(pplns->get_blocks_received("miner_D"), 1);
    ASSERT_EQ(pplns->get_blocks_mined("miner_D"), 1);    

    pplns->handle_share("miner_B", Share(Share::Property::none));
    pplns->handle_share("miner_A", Share(Share::Property::none));
    pplns->handle_share("miner_A", Share(Share::Property::none));
    pplns->handle_share("miner_A", Share(Share::Property::none));
    pplns->handle_share("miner_A", Share(Share::Property::valid_block));
    ASSERT_EQ(pplns->get_blocks_received("miner_A"), 1);
    ASSERT_EQ(pplns->get_blocks_mined("miner_A"), 1);
    ASSERT_EQ(pplns->get_blocks_mined("miner_B"), 0);
    ASSERT_EQ(pplns->get_blocks_received("miner_B"), 0);

    pplns->handle_share("miner_B", Share(Share::Property::none));
    pplns->handle_share("miner_C", Share(Share::Property::valid_block));
    ASSERT_EQ(pplns->get_blocks_mined("miner_C"), 1);
    ASSERT_NEAR(pplns->get_blocks_received("miner_C"), 0.3333, 0.0001);
    ASSERT_NEAR(pplns->get_blocks_received("miner_B"), 0.3333, 0.0001);

    pplns->handle_share("miner_A", Share(Share::Property::none));
    pplns->handle_share("miner_A", Share(Share::Property::none));
    pplns->handle_share("miner_B", Share(Share::Property::valid_block));
    ASSERT_EQ(pplns->get_record("miner_A")->get_blocks_mined(), 1);
    ASSERT_NEAR(pplns->get_blocks_received("miner_A"), 1.9999, 0.0001);
    ASSERT_EQ(pplns->get_blocks_mined("miner_B"), 1);
    ASSERT_NEAR(pplns->get_blocks_received("miner_B"), 0.6666, 0.0001);
    ASSERT_EQ(pplns->get_blocks_mined("miner_C"), 1);
    ASSERT_NEAR(pplns->get_blocks_received("miner_C"), 0.3333, 0.0001);

    ASSERT_EQ(pplns->get_record("miner_A")->get_uncles_received(), 0);
    pplns->handle_share("miner_A", Share(Share::Property::uncle | Share::Property::valid_block));

    ASSERT_EQ(pplns->get_record("miner_A")->get_uncles_mined(), 1);
    ASSERT_NEAR(pplns->get_record("miner_A")->get_uncles_received(), 0.6666, 0.0001);
    ASSERT_NEAR(pplns->get_record("miner_B")->get_uncles_received(), 0.3333, 0.0001);
    
    pplns->handle_share("miner_A", Share(Share::Property::uncle | Share::Property::valid_block));
    pplns->handle_share("miner_B", Share(Share::Property::uncle | Share::Property::valid_block));
    pplns->handle_share("miner_D", Share(Share::Property::uncle | Share::Property::valid_block));
    ASSERT_NEAR(pplns->get_record("miner_A")->get_uncles_mined(), 2, 0.0001);
    ASSERT_EQ(pplns->get_record("miner_B")->get_uncles_mined(), 1);
    ASSERT_EQ(pplns->get_record("miner_C")->get_uncles_mined(), 0);
    ASSERT_NEAR(pplns->get_record("miner_A")->get_uncles_received(), 2.3333, 0.0001);
    ASSERT_NEAR(pplns->get_record("miner_B")->get_uncles_received(), 1.3333, 0.0001);
    ASSERT_EQ(pplns->get_blocks_mined("miner_B"), 1);
}

TEST(PPLNSRewardScheme, last_n_shares) {
    auto simulation = Simulation::from_string(pplns_simulation_string);
    ASSERT_EQ(simulation.pools.size(), 1);
    auto reward_config = simulation.pools[0].reward_scheme_config;
    ASSERT_EQ(reward_config.scheme_type, "pplns");
    ASSERT_EQ(reward_config.params["n"], 3);
    auto params = reward_config.params;
    std::unique_ptr<PPLNSRewardScheme> pplns_ptr = std::unique_ptr<PPLNSRewardScheme>(new PPLNSRewardScheme(reward_config.params));
    auto base_ptr = RewardSchemeFactory::create(reward_config.scheme_type, reward_config.params);
    auto pplns = pplns_ptr.get();
    
    auto pool = simulation.pools[0];
    auto network = std::make_shared<Network>(simulation.network_difficulty);
    auto mining_pool = MiningPool::create("pool", pool.difficulty, pool.uncle_block_prob, std::move(pplns_ptr), network);

    pplns->handle_share("miner_A", Share(Share::Property::none));
    ASSERT_EQ(pplns->get_last_n_shares_size(), 1);

    pplns->handle_share("miner_B", Share(Share::Property::none));
    ASSERT_EQ(pplns->get_last_n_shares_size(), 2);

    pplns->handle_share("miner_C", Share(Share::Property::none));
    ASSERT_EQ(pplns->get_last_n_shares_size(), 3);

    pplns->handle_share("miner_B", Share(Share::Property::none));
    ASSERT_EQ(pplns->get_last_n_shares_size(), 3);

    pplns->handle_share("miner_B", Share(Share::Property::none));
    ASSERT_EQ(pplns->get_last_n_shares_size(), 3);

    pplns->handle_share("miner_B", Share(Share::Property::valid_block));
    ASSERT_EQ(pplns->get_last_n_shares_size(), 3);
}

TEST(PPSRewardScheme, handle_share) {
    auto simulation = Simulation::from_string(pps_simulation_string);
    ASSERT_EQ(simulation.pools.size(), 1);
    auto reward_config = simulation.pools[0].reward_scheme_config;
    ASSERT_EQ(reward_config.scheme_type, "pps");
    ASSERT_EQ(reward_config.params["pool_fee"], 0);
    auto params = reward_config.params["pool_fee"];
    auto pps_uptr = RewardSchemeFactory::create(reward_config.scheme_type,
                                                reward_config.params);
    auto pps = pps_uptr.get();

    auto pool_config = simulation.pools[0];
    auto network = get_sample_network();
    auto pool = MiningPool::create("pool", pool_config.difficulty, pool_config.uncle_block_prob, std::move(pps_uptr), network);

    ASSERT_EQ(pool->get_network(), network);
    ASSERT_EQ(pps->get_mining_pool(), pool);

    pps->handle_share("miner_A", Share(Share::Property::valid_block));
    ASSERT_EQ(pps->get_blocks_mined("miner_A"), 1);
    ASSERT_FLOAT_EQ(pps->get_blocks_received("miner_A"), 0.1);
    
    pps->handle_share("miner_A", Share(Share::Property::none));
    pps->handle_share("miner_B", Share(Share::Property::none));
    pps->handle_share("miner_A", Share(Share::Property::valid_block));
    pps->handle_share("miner_B", Share(Share::Property::none));
    pps->handle_share("miner_C", Share(Share::Property::valid_block));
    ASSERT_EQ(pps->get_blocks_mined("miner_C"), 1);
    ASSERT_FLOAT_EQ(pps->get_blocks_received("miner_C"), 0.1);
    ASSERT_EQ(pps->get_blocks_mined("miner_C"), 1);
    ASSERT_FLOAT_EQ(pps->get_blocks_received("miner_B"), 0.2);
    ASSERT_EQ(pps->get_blocks_mined("miner_B"), 0);
    ASSERT_FLOAT_EQ(pps->get_blocks_received("miner_A"), 0.3);
    ASSERT_EQ(pps->get_blocks_mined("miner_A"), 2);

    pps->handle_share("miner_A", Share(Share::Property::none));
    pps->handle_share("miner_B", Share(Share::Property::valid_block));
    pps->handle_share("miner_A", Share(Share::Property::valid_block));
    ASSERT_EQ(pps->get_blocks_mined("miner_A"), 3);
    ASSERT_FLOAT_EQ(pps->get_blocks_received("miner_A"), 0.5);
    ASSERT_EQ(pps->get_blocks_mined("miner_B"), 1);
    ASSERT_FLOAT_EQ(pps->get_blocks_received("miner_B"), 0.3);
}


TEST(PROPRewardScheme, handle_share) {
    auto simulation = Simulation::from_string(prop_simulation_string);
    ASSERT_EQ(simulation.pools.size(), 1);
    auto reward_config = simulation.pools[0].reward_scheme_config;
    ASSERT_EQ(reward_config.scheme_type, "prop");
    ASSERT_EQ(reward_config.params["pool_fee"], 0);
    auto params = reward_config.params["pool_fee"];
    auto prop_uptr = RewardSchemeFactory::create(reward_config.scheme_type,
                                                reward_config.params);
    auto prop = prop_uptr.get();
    auto pool_config = simulation.pools[0];
    auto network = get_sample_network();
    auto pool = MiningPool::create("pool", pool_config.difficulty, pool_config.uncle_block_prob, std::move(prop_uptr), network);

    ASSERT_EQ(pool->get_network(), network);
    ASSERT_EQ(prop->get_mining_pool(), pool); 

    prop->handle_share("miner_A", Share(Share::Property::none));
    prop->handle_share("miner_B", Share(Share::Property::none));
    prop->handle_share("miner_A", Share(Share::Property::none));
    prop->handle_share("miner_B", Share(Share::Property::none));
    prop->handle_share("miner_C", Share(Share::Property::none));
    prop->handle_share("miner_C", Share(Share::Property::none));
    prop->handle_share("miner_D", Share(Share::Property::none));
    prop->handle_share("miner_D", Share(Share::Property::none));
    prop->handle_share("miner_D", Share(Share::Property::none));
    prop->handle_share("miner_E", Share(Share::Property::valid_block));
    ASSERT_FLOAT_EQ(prop->get_blocks_received("miner_A"), 0.2);
    ASSERT_FLOAT_EQ(prop->get_blocks_received("miner_B"), 0.2);
    ASSERT_FLOAT_EQ(prop->get_blocks_received("miner_C"), 0.2);
    ASSERT_FLOAT_EQ(prop->get_blocks_received("miner_D"), 0.3);
    ASSERT_FLOAT_EQ(prop->get_blocks_received("miner_E"), 0.1);
    
    prop->handle_share("miner_B", Share(Share::Property::valid_block));
    ASSERT_FLOAT_EQ(prop->get_blocks_received("miner_B"), 1.2);

    prop->handle_share("miner_F", Share(Share::Property::none));
    prop->handle_share("miner_G", Share(Share::Property::valid_block));
    ASSERT_FLOAT_EQ(prop->get_blocks_received("miner_F"), 0.5);
    ASSERT_FLOAT_EQ(prop->get_blocks_received("miner_G"), 0.5);

    prop->handle_share("miner_A", Share(Share::Property::none));
    prop->handle_share("miner_B", Share(Share::Property::uncle | Share::Property::valid_block));
    ASSERT_FLOAT_EQ(prop->get_record("miner_A")->get_uncles_received(), 0.5);
    ASSERT_FLOAT_EQ(prop->get_record("miner_B")->get_uncles_received(), 0.5);

    prop->handle_share("miner_B", Share(Share::Property::none));
    prop->handle_share("miner_B", Share(Share::Property::uncle | Share::Property::valid_block));
    ASSERT_FLOAT_EQ(prop->get_record("miner_A")->get_uncles_received(), 0.75);
    ASSERT_FLOAT_EQ(prop->get_record("miner_A")->get_uncles_mined(), 0);
    ASSERT_FLOAT_EQ(prop->get_record("miner_B")->get_uncles_received(), 1.25);
    ASSERT_FLOAT_EQ(prop->get_record("miner_B")->get_uncles_mined(),2);
    
    prop->handle_share("miner_A", Share(Share::Property::valid_block));
    ASSERT_FLOAT_EQ(prop->get_blocks_received("miner_A"), 0.6);
    ASSERT_FLOAT_EQ(prop->get_blocks_mined("miner_A"), 1);

    prop->handle_share("miner_B", Share(Share::Property::none));
    prop->handle_share("miner_C", Share(Share::Property::none));
    prop->handle_share("miner_A", Share(Share::Property::none));
    prop->handle_share("miner_A", Share(Share::Property::uncle | Share::Property::valid_block));
    ASSERT_FLOAT_EQ(prop->get_record("miner_A")->get_uncles_received(), 1.25);
    ASSERT_FLOAT_EQ(prop->get_record("miner_A")->get_uncles_mined(), 1);
    ASSERT_FLOAT_EQ(prop->get_record("miner_B")->get_uncles_received(), 1.5);
    ASSERT_FLOAT_EQ(prop->get_record("miner_B")->get_uncles_mined(), 2);
    ASSERT_FLOAT_EQ(prop->get_record("miner_C")->get_uncles_received(), 0.25);
    ASSERT_FLOAT_EQ(prop->get_record("miner_C")->get_uncles_mined(), 0);
    ASSERT_FLOAT_EQ(prop->get_record("miner_F")->get_uncles_received(), 0);
    ASSERT_FLOAT_EQ(prop->get_record("miner_F")->get_uncles_mined(), 0);
}

TEST(EventQueue, events_ordering) {
    EventQueue eq;
    ASSERT_TRUE(eq.is_empty());
    eq.schedule(Event("ev1", 2));
    ASSERT_FALSE(eq.is_empty());
    eq.schedule(Event("ev2", 1));
    eq.schedule(Event("ev3", 5));
    eq.schedule(Event("ev4", 4));
    ASSERT_EQ(eq.pop().miner_address, "ev2");
    ASSERT_EQ(eq.pop().miner_address, "ev1");
    ASSERT_EQ(eq.pop().miner_address, "ev4");
    ASSERT_EQ(eq.pop().miner_address, "ev3");
}

TEST(Simulator, schedule_miner) {
    auto simulation = Simulation::from_string(simulation_string);
    auto pool = MiningPool::create("pool", 50, 0.001, get_mock_reward_scheme(), get_sample_network());
    auto miner = Miner::create("address", 25, get_mock_share_handler(), get_sample_network());
    miner->join_pool(pool);
    auto random = std::make_shared<MockRandom>();
    auto simulator = std::make_shared<Simulator>(simulation, random);
    ASSERT_EQ(simulator->get_events_count(), 0);
    EXPECT_CALL(*random, drand48()).Times(1).WillOnce(testing::Return(0.3));
    simulator->schedule_miner(miner);
    ASSERT_NE(simulator->get_events_count(), 0);
    auto event = simulator->get_next_event();
    ASSERT_EQ(event.miner_address, "address");
    // 25 / 50 = 0.5
    ASSERT_FLOAT_EQ(event.time, -log(0.3) / 0.5);
}

TEST(Simulator, process_event) {
    auto simulation = Simulation::from_string(simulation_string);
    auto pool = MiningPool::create("pool", 50, 0.001, get_mock_reward_scheme(), get_sample_network());
    auto miner = std::make_shared<MockMiner>("address", 25, get_sample_network());
    miner->join_pool(pool);
    auto random = std::make_shared<MockRandom>();  
    auto simulator = std::make_shared<Simulator>(simulation, random);
    simulator->add_miner(miner);
    Event event(miner->get_address(), 5);
    ASSERT_EQ(simulator->get_blocks_mined(), 0);
    ASSERT_EQ(simulator->get_current_time(), 0);
    // drand48() called once in process_event and once in schedule_miner
    EXPECT_CALL(*random, drand48()).Times(2).WillRepeatedly(testing::Return(0.3));
    // 0.3 < 0.5 -> network share
    EXPECT_CALL(*miner, process_share(Share(true))).Times(1);
    simulator->process_event(event);
    ASSERT_EQ(simulator->get_blocks_mined(), 1);
    ASSERT_EQ(simulator->get_current_time(), 5);

    Event event2(miner->get_address(), 10);
    EXPECT_CALL(*random, drand48()).Times(2).WillRepeatedly(testing::Return(0.8));
    // 0.8 > 0.5 -> not network share
    EXPECT_CALL(*miner, process_share(Share(false))).Times(1);
    simulator->process_event(event2);
    ASSERT_EQ(simulator->get_blocks_mined(), 1);
    ASSERT_EQ(simulator->get_current_time(), 10);
}

TEST(Simulator, initialize) {
    auto simulator = get_sample_simulator();
    simulator->initialize();
    ASSERT_EQ(simulator->get_pools_count(), 1);
    ASSERT_EQ(simulator->get_miners_count(), 100);
    ASSERT_EQ(simulator->get_network()->get_pools().size(), 1);
}

TEST(Simulator, schedule_all) {
    auto simulator = get_sample_simulator();
    simulator->initialize();
    ASSERT_EQ(simulator->get_events_count(), 0);
    simulator->schedule_all();
    ASSERT_EQ(simulator->get_events_count(), 100);
}

TEST(Random, UniformDistribution) {
    auto args = R"({"low": 0.0, "high": 10.0})"_json;
    auto dist = DistributionFactory::create("uniform", args);
    double value = dist->get();
    ASSERT_GE(value, 0.0);
    ASSERT_LE(value, 10.0);
}

TEST(Random, NormalDistribution) {
  auto args = R"({"mean": 0.0, "stddev": 1.0})"_json;
  auto dist = DistributionFactory::create("normal", args);
  double value = dist->get();
  ASSERT_GE(value, -5.0);
  ASSERT_LE(value, 5.0);
}

TEST(MinerCreatorStopCondition, TotalHashrate) {
    auto args = R"({"value": 20})"_json;
    auto cond = MinerCreatorStopConditionFactory::create("total_hashrate", args);
    ASSERT_TRUE(cond->should_stop(MinerCreationState(0, 100)));
    ASSERT_FALSE(cond->should_stop(MinerCreationState(3, 10)));
}

TEST(MinerCreatorStopCondition, MinersCount) {
    auto args = R"({"value": 10})"_json;
    auto cond = MinerCreatorStopConditionFactory::create("miners_count", args);
    ASSERT_TRUE(cond->should_stop(MinerCreationState(15, 100)));
    ASSERT_FALSE(cond->should_stop(MinerCreationState(5, 50)));
}

TEST(MinerCreator, MinerCreationState) {
    MinerCreationState state;
    ASSERT_EQ(state.miners_count, 0);
    ASSERT_EQ(state.total_hashrate, 0);
}

TEST(MinerCreator, RandomMinerCreator) {
    auto args = R"({
        "behavior": {"name": "default", "params": {}},
        "hashrate": {
            "distribution": "normal",
            "params": {"mean": 10.0, "stddev": 1.5, "minimum": 2.0, "maximum": 40.0}
        },
        "stop_condition": {
            "type": "miners_count",
            "params": {"value": 100}
        }
    })"_json;
    auto creator = MinerCreatorFactory::create("random", get_sample_network());
    auto miners = creator->create_miners(args);
    ASSERT_EQ(miners.size(), 100);

    args["stop_condition"]["type"] = "total_hashrate";
    miners = creator->create_miners(args);
    ASSERT_FALSE(miners.empty());
    uint64_t total_hashrate = 0;
    for (auto miner : miners) {
        total_hashrate += miner->get_hashrate();
        ASSERT_GE(miner->get_hashrate(), 2.0);
        ASSERT_LE(miner->get_hashrate(), 40.0);
    }
    ASSERT_GE(total_hashrate, 100);
}

TEST(MinerCreator, CSVMinerCreator) {
    auto args = R"({
        "behavior": {"name": "default", "params": {}},
        "path": "fixtures/sample_miners.csv"
    })"_json;
    auto creator = MinerCreatorFactory::create("csv", get_sample_network());
    auto miners = creator->create_miners(args);
    ASSERT_EQ(miners.size(), 3);
    ASSERT_FLOAT_EQ(miners[0]->get_hashrate(), 1);
    ASSERT_FLOAT_EQ(miners[1]->get_hashrate(), 2);
    ASSERT_FLOAT_EQ(miners[2]->get_hashrate(), 3);
}

TEST(MinerCreator, InlineMinerCreator) {
    auto args = R"({
        "miners": [
            {"hashrate": 1.0, "address": "0x7da82c7ab4771ff031b66538d2fb9b0b047f6cf9"},
            {"address": "0xaa1a6e3e6ef20068f7f8d8c835d2d22fd5116444",
             "behavior": {"name": "share_donation"},
             "hashrate": 10}
        ]
    })"_json;
    auto creator = MinerCreatorFactory::create("inline", get_sample_network());
    auto miners = creator->create_miners(args);
    ASSERT_EQ(miners.size(), 2);
    ASSERT_EQ(miners[0]->get_handler_name(), "default");
    ASSERT_FLOAT_EQ(miners[0]->get_hashrate(), 1);
    ASSERT_EQ(miners[0]->get_address(), "0x7da82c7ab4771ff031b66538d2fb9b0b047f6cf9");
    ASSERT_FLOAT_EQ(miners[1]->get_hashrate(), 10);
    ASSERT_EQ(miners[1]->get_handler_name(), "share_donation");
    ASSERT_EQ(miners[1]->get_address(), "0xaa1a6e3e6ef20068f7f8d8c835d2d22fd5116444");
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
