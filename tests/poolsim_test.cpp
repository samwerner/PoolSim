#include <gtest/gtest.h>

#include "miner.h"
#include "mining_pool.h"


TEST(Miner, accessors) {
  auto miner = std::make_shared<Miner>("random_address", 123);
  ASSERT_EQ(miner->get_address(), "random_address");
  ASSERT_EQ(miner->get_hashrate(), 123);
}

TEST(Miner, join_pool) {
  auto miner = std::make_shared<Miner>("random_address", 123);
  auto pool1 = std::make_shared<MiningPool>();
  auto pool2 = std::make_shared<MiningPool>();
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


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
