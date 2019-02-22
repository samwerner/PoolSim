#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "miner.h"
#include "share.h"
#include "mining_pool.h"
#include "event_queue.h"
#include "simulation.h"
#include "simulator.h"
#include "random.h"


const std::string simulation_string = R"({
  "blocks": 5,
  "networkDifficulty": 100,
  "pools": [{
    "difficulty": 10,
    "miners": {
      "generator": "csv",
      "params": {"path": "miners.csv"}
    }
  }]
})";

const nlohmann::json random_miners_params = R"({
  "hashrate": {
    "distribution": "normal",
    "params": {"mean": 10.0, "variance": 30.0}
  },
  "stop_condition": {
    "type": "miners_count",
    "params": {"value": 100}
  }
})"_json;


class MockRandom: public Random {
public:
  MOCK_METHOD0(drand48, double());
  MOCK_METHOD0(get_address, std::string());
  MOCK_METHOD0(get_random_engine, std::shared_ptr<std::default_random_engine>());
};

class MockMiner: public Miner {
public:
  MockMiner(const std::string& address, double hashrate): Miner(address, hashrate) {}
  MOCK_METHOD1(process_share, void(const Share& share));
};


Simulation get_sample_simulation() {
  auto simulation_json = nlohmann::json::parse(simulation_string);
  simulation_json["pools"][0]["miners"]["generator"] = "random";
  simulation_json["pools"][0]["miners"]["params"] = random_miners_params;
  return simulation_json.get<Simulation>();
}

Simulator get_sample_simulator() {
  return Simulator(get_sample_simulation());
}


TEST(Share, equality) {
  ASSERT_EQ(Share(true), Share(true));
  ASSERT_EQ(Share(false), Share(false));
  ASSERT_NE(Share(true), Share(false));
}


TEST(Miner, accessors) {
  auto miner = std::make_shared<Miner>("random_address", 123);
  ASSERT_EQ(miner->get_address(), "random_address");
  ASSERT_EQ(miner->get_hashrate(), 123);
}

TEST(Miner, join_pool) {
  auto miner = std::make_shared<Miner>("random_address", 123);
  auto pool1 = std::make_shared<MiningPool>(100);
  auto pool2 = std::make_shared<MiningPool>(100);
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

TEST(Simulation, from_string) {
  auto simulation = Simulation::from_string(simulation_string);
  ASSERT_EQ(simulation.blocks, 5);
  ASSERT_EQ(simulation.network_difficulty, 100);
  ASSERT_EQ(simulation.pools.size(), 1);
  auto pool = simulation.pools[0];
  ASSERT_EQ(pool.difficulty, 10);
  auto miner_config = pool.miner_config;
  ASSERT_EQ(miner_config.generator, "csv");
  ASSERT_EQ(miner_config.params["path"], "miners.csv");
}

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
  auto pool = std::make_shared<MiningPool>(50);
  auto miner = std::make_shared<Miner>("address", 25);
  miner->join_pool(pool);
  auto random = std::make_shared<MockRandom>();
  Simulator simulator(simulation, random);
  ASSERT_EQ(simulator.get_events_count(), 0);
  EXPECT_CALL(*random, drand48()).Times(1).WillOnce(testing::Return(0.3));
  simulator.schedule_miner(miner);
  ASSERT_NE(simulator.get_events_count(), 0);
  auto event = simulator.get_next_event();
  ASSERT_EQ(event.miner_address, "address");
  // 25 / 50 = 0.5
  ASSERT_FLOAT_EQ(event.time, -log(0.3) / 0.5);
}

TEST(Simulator, process_event) {
  auto simulation = Simulation::from_string(simulation_string);
  auto pool = std::make_shared<MiningPool>(50);
  auto miner = std::make_shared<MockMiner>("address", 25);
  miner->join_pool(pool);
  auto random = std::make_shared<MockRandom>();
  Simulator simulator(simulation, random);
  simulator.add_miner(miner);
  Event event(miner->get_address(), 5);
  ASSERT_EQ(simulator.get_blocks_mined(), 0);
  ASSERT_EQ(simulator.get_current_time(), 0);
  // drand48() called once in process_event and once in schedule_miner
  EXPECT_CALL(*random, drand48()).Times(2).WillRepeatedly(testing::Return(0.3));
  // 0.3 < 0.5 -> network share
  EXPECT_CALL(*miner, process_share(Share(true))).Times(1);
  simulator.process_event(event);
  ASSERT_EQ(simulator.get_blocks_mined(), 1);
  ASSERT_EQ(simulator.get_current_time(), 5);

  Event event2(miner->get_address(), 10);
  EXPECT_CALL(*random, drand48()).Times(2).WillRepeatedly(testing::Return(0.8));
  // 0.8 > 0.5 -> not network share
  EXPECT_CALL(*miner, process_share(Share(false))).Times(1);
  simulator.process_event(event2);
  ASSERT_EQ(simulator.get_blocks_mined(), 1);
  ASSERT_EQ(simulator.get_current_time(), 10);
}

TEST(Simulator, initialize) {
  auto simulator = get_sample_simulator();
  simulator.initialize();
  ASSERT_EQ(simulator.get_pools_count(), 1);
  ASSERT_EQ(simulator.get_miners_count(), 100);
}

TEST(Simulator, schedule_all) {
  auto simulator = get_sample_simulator();
  simulator.initialize();
  ASSERT_EQ(simulator.get_events_count(), 0);
  simulator.schedule_all();
  ASSERT_EQ(simulator.get_events_count(), 100);
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

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
