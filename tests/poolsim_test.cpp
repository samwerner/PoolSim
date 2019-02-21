#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "miner.h"
#include "mining_pool.h"
#include "event_queue.h"
#include "simulation.h"
#include "simulator.h"
#include "random.h"


const std::string simulation_string = R"({
  "rounds": 5,
  "networkDifficulty": 100
})";


class DummyRandom: public Random {
public:
  MOCK_METHOD0(drand48, double());
};


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
  ASSERT_EQ(simulation.rounds, 5);
  ASSERT_EQ(simulation.network_difficulty, 100);
}

TEST(SystemRandom, initialization) {
  ASSERT_THROW(SystemRandom::getInstance(), RandomInitException);
  SystemRandom::initialize(0);
  ASSERT_NO_THROW(SystemRandom::getInstance());
  // NOTE: expected value with seed = 0
  ASSERT_FLOAT_EQ(SystemRandom::getInstance().drand48(), 0.170828);
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
  auto pool = std::make_shared<MiningPool>(100);
  auto miner = std::make_shared<Miner>("address", 50);
  miner->join_pool(pool);
  auto random = std::make_shared<DummyRandom>();
  Simulator simulator(simulation, random);
  ASSERT_TRUE(simulator.get_event_queue().is_empty());
  EXPECT_CALL(*random, drand48()).Times(1).WillOnce(testing::Return(0.3));
  simulator.schedule_miner(miner);
  ASSERT_FALSE(simulator.get_event_queue().is_empty());
  auto event = simulator.get_event_queue().get_top();
  ASSERT_EQ(event.miner_address, "address");
  // 50 / 100 = 0.5
  ASSERT_FLOAT_EQ(event.time, -log(0.3) / 0.5);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
