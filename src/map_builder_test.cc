#include "tcs/util/map_builder.h"

#include <gtest/gtest.h>

using namespace tcs;
using namespace std;

class MapBuilderTest : public ::testing::Test {
 protected:
  void SetUp() override { builder.Init(); }

  MapBuilder& builder = MapBuilder::GetInstance();
};

TEST_F(MapBuilderTest, AddPointAndPath) {
  auto point0 = builder.AddPoint({0, 0, 0});
  auto point1 = builder.AddPoint({0, 0, 0});
  auto path2 = builder.AddPath(point0, point1, 1);

  unique_ptr<Map> map{builder.Build()};

  auto point_ptr0 = map->GetPoint(point0);
  ASSERT_TRUE(point_ptr0);
  ASSERT_EQ(point_ptr0->GetOutPaths().size(), 1);
  ASSERT_NE(point_ptr0->GetOutPaths().find(path2),
            point_ptr0->GetOutPaths().end());
}

TEST_F(MapBuilderTest, RemovePoint) {
  auto point0 = builder.AddPoint({0, 0, 0});
  auto point1 = builder.AddPoint({0, 0, 0});
  auto path2 = builder.AddPath(point0, point1, 1);

  builder.RemovePoint(point0);

  unique_ptr<Map> map{builder.Build()};

  auto point_ptr1 = map->GetPoint(point1);
  ASSERT_TRUE(point_ptr1);
  ASSERT_EQ(point_ptr1->GetInPaths().size(), 0);
}

TEST_F(MapBuilderTest, RemovePath) {
  auto point0 = builder.AddPoint({0, 0, 0});
  auto point1 = builder.AddPoint({0, 0, 0});
  auto path2 = builder.AddPath(point0, point1, 1);

  builder.RemovePath(path2);

  unique_ptr<Map> map{builder.Build()};

  auto point_ptr1 = map->GetPoint(point1);
  ASSERT_TRUE(point_ptr1);
  ASSERT_EQ(point_ptr1->GetInPaths().size(), 0);
}

TEST_F(MapBuilderTest, AddLocation) {
  auto point0 = builder.AddPoint({0, 0, 0});
  auto point1 = builder.AddPoint({0, 0, 0});
  auto path2 = builder.AddPath(point0, point1, 1);

  auto location3 = builder.AddLocation({"Load"}, {point0});
  builder.Link(point1, location3);

  unique_ptr<Map> map{builder.Build()};

  auto location_ptr3 = map->GetLocation(location3);
  ASSERT_TRUE(location_ptr3);
  ASSERT_EQ(location_ptr3->GetLinkedPoints().size(), 2);
}

TEST_F(MapBuilderTest, UnlinkLocation) {
  auto point0 = builder.AddPoint({0, 0, 0});
  auto point1 = builder.AddPoint({0, 0, 0});
  auto path2 = builder.AddPath(point0, point1, 1);

  auto location3 = builder.AddLocation({"Load"}, {point0});
  builder.Link(point1, location3);
  builder.Unlink(point0, location3);

  unique_ptr<Map> map{builder.Build()};

  auto location_ptr3 = map->GetLocation(location3);
  auto point_ptr0 = map->GetPoint(point0);
  auto point_ptr1 = map->GetPoint(point1);
  ASSERT_TRUE(location_ptr3);
  ASSERT_TRUE(point_ptr0);
  ASSERT_TRUE(point_ptr1);
  ASSERT_EQ(location_ptr3->GetLinkedPoints().size(), 1);
  ASSERT_FALSE(point_ptr0->GetLinkedLocation().has_value());
  ASSERT_TRUE(point_ptr1->GetLinkedLocation().has_value());
}

TEST_F(MapBuilderTest, AddBlock) {
  auto point0 = builder.AddPoint({0, 0, 0});
  auto point1 = builder.AddPoint({0, 0, 0});
  auto point2 = builder.AddPoint({0, 0, 0});
  auto path3 = builder.AddPath(point0, point1, 1);
  auto path4 = builder.AddPath(point2, point1, 1);

  auto block5 = builder.AddBlock({point0, path3});
  auto block6 = builder.AddBlock({point2});
  builder.AddToBlock(path4, block6);

  unique_ptr<Map> map{builder.Build()};

  auto block_ptr5 = map->GetBlock(block5);
  auto block_ptr6 = map->GetBlock(block6);
  ASSERT_TRUE(block_ptr5);
  ASSERT_TRUE(block_ptr6);
  ASSERT_NE(block_ptr5->GetResources().find(point0),
            block_ptr5->GetResources().end());
  ASSERT_EQ(block_ptr6->GetResources().find(point0),
            block_ptr6->GetResources().end());
  ASSERT_EQ(block_ptr5->GetResources().size(), 2);
  ASSERT_EQ(block_ptr6->GetResources().size(), 2);
}

TEST_F(MapBuilderTest, RemoveBlock) {
  auto point0 = builder.AddPoint({0, 0, 0});
  auto point1 = builder.AddPoint({0, 0, 0});
  auto point2 = builder.AddPoint({0, 0, 0});
  auto path3 = builder.AddPath(point0, point1, 1);
  auto path4 = builder.AddPath(point2, point1, 1);

  auto block5 = builder.AddBlock({point0, path3});
  auto block6 = builder.AddBlock({point2});
  builder.AddToBlock(path4, block6);

  builder.RemoveFromBlock(point0, block5);
  builder.RemoveBlock(block6);

  unique_ptr<Map> map{builder.Build()};

  auto block_ptr5 = map->GetBlock(block5);
  auto block_ptr6 = map->GetBlock(block6);
  ASSERT_TRUE(block_ptr5);
  ASSERT_FALSE(block_ptr6);
  ASSERT_EQ(block_ptr5->GetResources().find(point0),
            block_ptr5->GetResources().end());
  ASSERT_EQ(block_ptr5->GetResources().size(), 1);
}