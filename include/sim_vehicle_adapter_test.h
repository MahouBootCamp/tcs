#ifndef SIM_VEHICLE_ADAPTER_TEST_H
#define SIM_VEHICLE_ADAPTER_TEST_H

#include "tcs/util/map_builder.h"
#include "tcs/vehicle/sim_vehicle_adapter.h"

tcs::Map* ProductVehicleAdapterTestMap() {
  auto builder = tcs::MapBuilder::GetInstance();
  builder.Init();

  auto vs = builder.AddPoint({0, 0, 0});  // 0
  auto vt = builder.AddPoint({1, 1, 0});  // 1
  auto vx = builder.AddPoint({2, 1, 0});  // 2
  auto vy = builder.AddPoint({1, 0, 0});  // 3
  auto vz = builder.AddPoint({2, 0, 0});  // 4
  auto vn = builder.AddPoint({3, 0, 0});  // 5
  auto vm = builder.AddPoint({4, 0, 0});  // 6

  auto est = builder.AddPath(vs, vt, 1000);  // 7
  auto esy = builder.AddPath(vs, vy, 500);   // 8
  auto ety = builder.AddPath(vt, vy, 200);   // 9
  auto etx = builder.AddPath(vt, vx, 100);   // 10
  auto exz = builder.AddPath(vx, vz, 400);   // 11
  auto eyt = builder.AddPath(vy, vt, 300);   // 12
  auto eyx = builder.AddPath(vy, vx, 900);   // 13
  auto eyz = builder.AddPath(vy, vz, 200);   // 14
  auto ezs = builder.AddPath(vz, vs, 700);   // 15
  auto ezx = builder.AddPath(vz, vx, 600);   // 16

  auto location0 = builder.AddLocation({"Load"}, {vs});        // 17
  auto location1 = builder.AddLocation({"Unload"}, {vx});      // 18
  auto location2 = builder.AddLocation({"Load"}, {vs, vn});    // 19
  auto location3 = builder.AddLocation({"Unload"}, {vx, vm});  // 20
  auto location4 = builder.AddLocation({"Unload"}, {vn, vm});  // 21
  auto location5 = builder.AddLocation({"Load"}, {vs, vt});    // 22
  auto location6 = builder.AddLocation({"Unload"}, {vy, vz});  // 23

  return builder.Build();
}

void SimVehicleAdapterTest() {
  std::unique_ptr<tcs::Map> map_{ProductVehicleAdapterTestMap()};
  tcs::SimVehicleAdapter adapter;

  tcs::Step step0{0, map_->get_point(0), map_->get_point(1), map_->get_path(7)};
  tcs::Step step1{1, map_->get_point(1), map_->get_point(2),
                  map_->get_path(10)};

  adapter.Enable();
  adapter.EnqueueCommand({step0, "Load", false});
  adapter.EnqueueCommand({step1, "Unload", true});
  std::this_thread::sleep_for(std::chrono::seconds(5));
  adapter.Disable();
}

#endif /* SIM_VEHICLE_ADAPTER_TEST_H */
