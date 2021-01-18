#include <gtest/gtest.h>

#include <memory>

#include "tcs/router/default_router.h"
#include "tcs/router/irouter.h"
#include "tcs/router/shortest_path_algorithm.h"
#include "tcs/util/map_builder.h"
#include "tcs/util/order_pool.h"

using namespace tcs;
using namespace std;

Map* BuildRouterTestMap() {
  auto builder = MapBuilder::GetInstance();
  builder.Init();

  auto vs = builder.AddPoint({0, 0, 0});  // 0
  auto vt = builder.AddPoint({1, 1, 0});  // 1
  auto vx = builder.AddPoint({2, 1, 0});  // 2
  auto vy = builder.AddPoint({1, 0, 0});  // 3
  auto vz = builder.AddPoint({2, 0, 0});  // 4
  auto vn = builder.AddPoint({3, 0, 0});  // 5
  auto vm = builder.AddPoint({4, 0, 0});  // 6

  auto est = builder.AddPath(vs, vt, 10);  // 7
  auto esy = builder.AddPath(vs, vy, 5);   // 8
  auto ety = builder.AddPath(vt, vy, 2);   // 9
  auto etx = builder.AddPath(vt, vx, 1);   // 10
  auto exz = builder.AddPath(vx, vz, 4);   // 11
  auto eyt = builder.AddPath(vy, vt, 3);   // 12
  auto eyx = builder.AddPath(vy, vx, 9);   // 13
  auto eyz = builder.AddPath(vy, vz, 2);   // 14
  auto ezs = builder.AddPath(vz, vs, 7);   // 15
  auto ezx = builder.AddPath(vz, vx, 6);   // 16

  auto location0 = builder.AddLocation({"Load"}, {vs});        // 17
  auto location1 = builder.AddLocation({"Unload"}, {vx});      // 18
  auto location2 = builder.AddLocation({"Load"}, {vs, vn});    // 19
  auto location3 = builder.AddLocation({"Unload"}, {vx, vm});  // 20
  auto location4 = builder.AddLocation({"Unload"}, {vn, vm});  // 21
  auto location5 = builder.AddLocation({"Load"}, {vs, vt});    // 22
  auto location6 = builder.AddLocation({"Unload"}, {vy, vz});  // 23

  return builder.Build();
}

class RouterTest : public ::testing::Test {
 protected:
  RouterTest()
      : map_{BuildRouterTestMap()},
        map_service_{make_unique<MapService>(map_.get(), mutex_)} {}
  void SetUp() override {
    order_pool_.reset(new OrderPool{map_.get()});
    transport_order_service_.reset(
        new TransportOrderService{order_pool_.get(), mutex_});
    router_.reset(
        new DefaultRouter{map_service_.get(), transport_order_service_.get()});
  }
  std::recursive_mutex mutex_;
  unique_ptr<Map> map_;
  unique_ptr<MapService> map_service_;
  unique_ptr<OrderPool> order_pool_;
  unique_ptr<TransportOrderService> transport_order_service_;
  unique_ptr<IRouter> router_;
};

TEST_F(RouterTest, AlgorithmTest) {
  ShortestPathAlgorithm algo(map_service_.get());
  auto pvs = map_->GetPoint(0);
  auto pvx = map_->GetPoint(2);
  auto pvz = map_->GetPoint(4);
  auto pvn = map_->GetPoint(5);
  auto pvm = map_->GetPoint(6);

  ASSERT_TRUE(algo.Routable(pvx, pvz));
  ASSERT_FALSE(algo.Routable(pvs, pvn));
  ASSERT_FALSE(algo.Routable(pvn, pvs));
  ASSERT_FALSE(algo.Routable(pvn, pvm));

  auto routess = algo.ComputeRoute(pvs, pvs);
  auto routemn = algo.ComputeRoute(pvm, pvn);
  auto routesx = algo.ComputeRoute(pvs, pvx);
  auto routesz = algo.ComputeRoute(pvs, pvz);

  ASSERT_EQ(routess->GetCost(), 0);
  ASSERT_FALSE(routemn.has_value());
  ASSERT_EQ(routesx->GetCost(), 9);
  ASSERT_EQ(routesz->GetSteps().size(), 2);
}

TEST_F(RouterTest, PointToPoint) {
  auto order0 = order_pool_->AddOrder({{17, "Load"}, {18, "Unload"}});
  ASSERT_TRUE(router_->ChechRoutability(order_pool_->GetOrder(order0)));
  auto order1 = order_pool_->AddOrder({{0, "Load"}, {6, "Unload"}});
  ASSERT_FALSE(router_->ChechRoutability(order_pool_->GetOrder(order1)));
}

TEST_F(RouterTest, LocationToLocation) {
  auto order0 = order_pool_->AddOrder({{19, "Load"}, {20, "Unload"}});
  auto order1 = order_pool_->AddOrder({{17, "Load"}, {21, "Unload"}});
  auto order2 = order_pool_->AddOrder({{22, "Load"}, {23, "Unload"}});
  auto order3 = order_pool_->AddOrder(
      {{22, "Load"}, {23, "Unload"}, {19, "Load"}, {20, "Unload"}});
  ASSERT_TRUE(router_->ChechRoutability(order_pool_->GetOrder(order0)));
  ASSERT_FALSE(router_->ChechRoutability(order_pool_->GetOrder(order1)));
  ASSERT_TRUE(router_->ChechRoutability(order_pool_->GetOrder(order2)));
  ASSERT_TRUE(router_->ChechRoutability(order_pool_->GetOrder(order3)));
}

TEST_F(RouterTest, TransportOrder) {
  auto pvx = map_->GetPoint(2);
  auto order0 = order_pool_->AddOrder(
      {{22, "Load"}, {23, "Unload"}, {19, "Load"}, {20, "Unload"}});
  auto route = router_->GetRoute(pvx, order_pool_->GetOrder(order0));
  ASSERT_TRUE(route.has_value());
}

TEST_F(RouterTest, TransportOrderNoRoute) {
  auto pvm = map_->GetPoint(6);
  auto order0 = order_pool_->AddOrder(
      {{22, "Load"}, {23, "Unload"}, {19, "Load"}, {20, "Unload"}});
  auto route = router_->GetRoute(pvm, order_pool_->GetOrder(order0));
  ASSERT_FALSE(route.has_value());
}