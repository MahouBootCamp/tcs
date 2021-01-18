#include "tcs/kernal.h"
#include "tcs/util/map_builder.h"
#include "sim_vehicle_adapter_test.h"

tcs::Map* BuildTestMap() {
  auto builder = tcs::MapBuilder::GetInstance();
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

  auto locationA = builder.AddLocation({tcs::kLoadOperation}, {vs});    // 15
  auto locationB = builder.AddLocation({tcs::kUnloadOperation}, {vz});  // 16

  auto vehicle = builder.AddVehicle();  // 17

  return builder.Build();
}

int main(int, char**) {
  auto map = BuildTestMap();
  std::unique_ptr<tcs::IKernal> kernal = std::make_unique<tcs::Kernal>(map);

  kernal->EnableVehicle(17, 2);
  kernal->Start();
  kernal->AddTransportOrder(
      {{15, tcs::kLoadOperation}, {16, tcs::kUnloadOperation}});
  std::this_thread::sleep_for(std::chrono::seconds(50000000));
  kernal->Exit();
  return 0;
  // SimVehicleAdapterTest();
  // return 0;
}