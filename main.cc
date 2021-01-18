#include "tcs/kernal.h"
#include "tcs/util/map_builder.h"

tcs::Map* BuildTestMap() {
  auto builder = tcs::MapBuilder::GetInstance();
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

int main(int, char**) {
  // Read/Initialize model
  // Initialize kernal
  // Set vehicle controller and set initial position for vehicles
  // Set kernal as running
  // Create transport orders
  // Exit

  auto map = BuildTestMap();

  std::unique_ptr<tcs::IKernal> kernal = std::make_unique<tcs::Kernal>(map);

  kernal->Start();
  std::this_thread::sleep_for(std::chrono::seconds(3));
  kernal->Exit();
  return 0;
}