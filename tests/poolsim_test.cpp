#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "miner.h"
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


const std::string qb_simulation_string = R"({
  "blocks": 5,
  "network_difficulty": 100,
  "pools": [{
    "uncle_block_prob": 0.001,
    "difficulty": 10,
    "reward_scheme": {"type": "qb", "params": {}},
    "miners": {
      "behavior": {"name": "default", "params": {}},
      "generator": "csv",
      "params": {"path": "miners.csv"}
    }
  }]
})";


const std::string simulation_string = R"({
  "blocks": 5,
  "network_difficulty": 100,
  "pools": [{
    "uncle_block_prob": 0.001,
    "difficulty": 10,
    "reward_scheme": {"type": "pps", "params": {}},
    "miners": {
      "behavior": {"name": "default", "params": {}},
      "generator": "csv",
      "params": {"path": "miners.csv"}
    }
  }]
})";

const nlohmann::json random_miners_params = R"({
  "behavior": {"name": "default", "params": {}},
  "hashrate": {
    "distribution": "normal",
    "params": {"mean": 10.0, "variance": 30.0}
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
  MOCK_METHOD0(get_random_engine, std::shared_ptr<std::default_random_engine>());
};

class MockMiner : public Miner {
public:
  MockMiner(const std::string& address, double hashrate): Miner(address, hashrate) {}
  MOCK_METHOD1(process_share, void(const Share& share));
};


class MockShareHandler : public ShareHandler {
public:
  MOCK_METHOD1(handle_share, void(const Share&));
};


class MockRewardScheme : public RewardScheme {
public:
    MOCK_METHOD2(handle_share, void(const std::string&, const Share&));
    MOCK_METHOD0(get_block_metadata, nlohmann::json());
    MOCK_METHOD1(get_miner_metadata, nlohmann::json (const std::string&));
};


Simulation get_sample_simulation() {
  auto simulation_json = nlohmann::json::parse(simulation_string);
  simulation_json["pools"][0]["miners"]["generator"] = "random";
  simulation_json["pools"][0]["miners"]["params"] = random_miners_params;
  return simulation_json.get<Simulation>();
}

std::shared_ptr<Simulator> get_sample_simulator(std::shared_ptr<Random> random) {
  return std::make_shared<Simulator>(get_sample_simulation(), random);
}

std::shared_ptr<Simulator> get_sample_simulator() {
  return std::make_shared<Simulator>(get_sample_simulation());
}



std::unique_ptr<ShareHandler> get_mock_share_handler() {
  return std::unique_ptr<ShareHandler>(new MockShareHandler);
}

std::unique_ptr<MockRewardScheme> get_mock_reward_scheme() {
  return std::unique_ptr<MockRewardScheme>(new MockRewardScheme);
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

TEST(Share, equality) {
  ASSERT_EQ(Share(Share::Property::network), Share(Share::Property::network));
  uint8_t flags = Share::Property::network | Share::Property::uncle;
  ASSERT_EQ(Share(flags), Share(flags));
  ASSERT_NE(Share(Share::Property::network), Share(Share::Property::uncle));
}

TEST(Share, flags) {
  ASSERT_TRUE(Share(Share::Property::network).is_network_share());
  ASSERT_TRUE(Share(Share::Property::uncle).is_uncle());
  ASSERT_FALSE(Share(Share::Property::uncle).is_network_share());
  uint8_t flags = Share::Property::network | Share::Property::uncle;
  ASSERT_TRUE(Share(flags).is_uncle());
  ASSERT_TRUE(Share(flags).is_network_share());
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
  auto miner_config = pool.miner_config;
  ASSERT_EQ(miner_config.generator, "csv");
  ASSERT_EQ(miner_config.params["path"], "miners.csv");
}


TEST(Miner, accessors) {
  auto miner = Miner::create("random_address", 123, get_mock_share_handler());
  ASSERT_EQ(miner->get_address(), "random_address");
  ASSERT_EQ(miner->get_hashrate(), 123);
}

TEST(Miner, join_pool) {
  auto miner = Miner::create("random_address", 123, get_mock_share_handler());
  auto pool1 = MiningPool::create(100, 0.001, get_mock_reward_scheme());
  auto pool2 = MiningPool::create(100, 0.001, get_mock_reward_scheme());
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

TEST(MiningPool, submit_share) {
  auto reward_scheme = get_mock_reward_scheme();
  auto random = std::make_shared<MockRandom>();
  MockRewardScheme* reward_scheme_ptr = reward_scheme.get();

  auto pool = MiningPool::create(100, 0.3, std::move(reward_scheme), random);

  EXPECT_CALL(*reward_scheme_ptr, handle_share("address", Share(Share::Property::none)));
  pool->submit_share("address", Share(Share::Property::none));

  EXPECT_CALL(*random, drand48()).WillOnce(testing::Return(0.5));
  EXPECT_CALL(*reward_scheme_ptr, handle_share("address", Share(Share::Property::network)));
  pool->submit_share("address", Share(Share::Property::network));

  EXPECT_CALL(*random, drand48()).WillOnce(testing::Return(0.2));
  EXPECT_CALL(*reward_scheme_ptr, handle_share("address", Share(Share::Property::network | Share::Property::uncle)));
  pool->submit_share("address", Share(Share::Property::network));
}

TEST(QBRewardScheme, update_record) {
nlohmann::json j;
std::unique_ptr<QBRewardScheme> reward_scheme(new QBRewardScheme(j));
auto random = std::make_shared<MockRandom>();
QBRewardScheme* reward_scheme_ptr = reward_scheme.get();

auto pool = MiningPool::create(100, 0.0, std::move(reward_scheme), random);

EXPECT_CALL(*random, drand48()).WillOnce(testing::Return(0.2));

pool->submit_share("address_A", Share(Share::Property::none));
pool->submit_share("address_B", Share(Share::Property::none));
pool->submit_share("address_C", Share(Share::Property::none));
pool->submit_share("address_A", Share(Share::Property::none));

ASSERT_EQ(reward_scheme_ptr->get_credits("address_A"), 200);
ASSERT_EQ(reward_scheme_ptr->get_credits("address_B"), 100);
ASSERT_EQ(reward_scheme_ptr->get_credits("address_C"), 100);

pool->submit_share("address_B", Share(Share::Property::none));
pool->submit_share("address_C", Share(Share::Property::none));
pool->submit_share("address_A", Share(Share::Property::none));
pool->submit_share("address_B", Share(Share::Property::none));
pool->submit_share("address_A", Share(Share::Property::none));
pool->submit_share("address_A", Share(Share::Property::none));

ASSERT_EQ(reward_scheme_ptr->get_credits("address_A"), 500);
ASSERT_EQ(reward_scheme_ptr->get_credits("address_B"), 300);
ASSERT_EQ(reward_scheme_ptr->get_credits("address_C"), 200);

pool->submit_share("address_B", Share(Share::Property::network));
ASSERT_EQ(reward_scheme_ptr->get_credits("address_A"), 100);
ASSERT_EQ(reward_scheme_ptr->get_credits("address_B"), 400);

pool->submit_share("address_B", Share(Share::Property::uncle));
ASSERT_EQ(reward_scheme_ptr->get_credits("address_B"), 500);
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
  auto pool = MiningPool::create(50, 0.001, get_mock_reward_scheme());
  auto miner = Miner::create("address", 25, get_mock_share_handler());
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
    auto pool = MiningPool::create(50, 0.001, get_mock_reward_scheme());
    auto miner = std::make_shared<MockMiner>("address", 25);
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
  auto args = R"({"mean": 0.0, "variance": 1.0})"_json;
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
      "params": {"mean": 10.0, "variance": 30.0}
    },
    "stop_condition": {
      "type": "miners_count",
      "params": {"value": 100}
    }
  })"_json;
  auto creator = MinerCreatorFactory::create("random");
  auto miners = creator->create_miners(args);
  ASSERT_EQ(miners.size(), 100);

  args["stop_condition"]["type"] = "total_hashrate";
  miners = creator->create_miners(args);
  ASSERT_FALSE(miners.empty());
  uint64_t total_hashrate = 0;
  for (auto miner : miners) {
    total_hashrate += miner->get_hashrate();
  }
  ASSERT_GE(total_hashrate, 100);
}

TEST(MinerCreator, CSVMinerCreator) {
  auto args = R"({
    "behavior": {"name": "default", "params": {}},
    "path": "tests/fixtures/sample_miners.csv"
  })"_json;
  auto creator = MinerCreatorFactory::create("csv");
  auto miners = creator->create_miners(args);
  ASSERT_EQ(miners.size(), 3);
  ASSERT_FLOAT_EQ(miners[0]->get_hashrate(), 1);
  ASSERT_FLOAT_EQ(miners[1]->get_hashrate(), 2);
  ASSERT_FLOAT_EQ(miners[2]->get_hashrate(), 3);
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
