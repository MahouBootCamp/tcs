#include "tcs/util/order_pool.h"

#include <gtest/gtest.h>

#include <memory>

#include "tcs/util/map_builder.h"

using namespace tcs;
using namespace std;

Map* BuildOrderPoolTestMap() {
  auto builder = MapBuilder::GetInstance();
  builder.Init();

  auto vs = builder.AddPoint({0, 0, 0});  // 0
  auto vt = builder.AddPoint({1, 1, 0});  // 1
  auto vx = builder.AddPoint({2, 1, 0});  // 2
  auto vy = builder.AddPoint({1, 0, 0});  // 3
  auto vz = builder.AddPoint({2, 0, 0});  // 4

  auto est = builder.AddPath(vs, vt, 10);  // 5
  auto esy = builder.AddPath(vs, vy, 5);   // 6
  auto ety = builder.AddPath(vt, vy, 2);   // 7
  auto etx = builder.AddPath(vt, vx, 1);   // 8
  auto exz = builder.AddPath(vx, vz, 4);   // 9
  auto eyt = builder.AddPath(vy, vt, 3);   // 10
  auto eyx = builder.AddPath(vy, vx, 9);   // 11
  auto eyz = builder.AddPath(vy, vz, 2);   // 12
  auto ezs = builder.AddPath(vz, vs, 7);   // 13
  auto ezx = builder.AddPath(vz, vx, 6);   // 14

  auto location1 = builder.AddLocation({"Load"}, {vs});    // 15
  auto location2 = builder.AddLocation({"Unload"}, {vx});  // 16

  return builder.Build();
}

class OrderPoolTest : public ::testing::Test {
 protected:
  void SetUp() override { map_.reset(BuildOrderPoolTestMap()); }
  unique_ptr<Map> map_;
};

TEST_F(OrderPoolTest, DefaultTest) {
  OrderPool order_pool(map_.get());
  auto order0 = order_pool.AddOrder({{15, "Load"}, {16, "Unload"}});
  auto order0_ptr = order_pool.GetOrder(order0);
  ASSERT_TRUE(order0_ptr);
  ASSERT_EQ(order0_ptr->GetDriveOrders().size(), 2);
  ASSERT_EQ(order0_ptr->GetDependencies().size(), 0);
}