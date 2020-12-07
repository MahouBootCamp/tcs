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

  auto point_ptr0 = map->get_point(point0);
  ASSERT_TRUE(point_ptr0);
  ASSERT_EQ(point_ptr0->get_out_paths().size(), 1);
  ASSERT_NE(point_ptr0->get_out_paths().find(path2),
            point_ptr0->get_out_paths().end());
}

TEST_F(MapBuilderTest, RemovePoint) {
  auto point0 = builder.AddPoint({0, 0, 0});
  auto point1 = builder.AddPoint({0, 0, 0});
  auto path2 = builder.AddPath(point0, point1, 1);

  builder.RemovePoint(point0);

  unique_ptr<Map> map{builder.Build()};

  auto point_ptr1 = map->get_point(point1);
  ASSERT_TRUE(point_ptr1);
  ASSERT_EQ(point_ptr1->get_in_paths().size(), 0);
}

TEST_F(MapBuilderTest, RemovePath) {
  auto point0 = builder.AddPoint({0, 0, 0});
  auto point1 = builder.AddPoint({0, 0, 0});
  auto path2 = builder.AddPath(point0, point1, 1);

  builder.RemovePath(path2);

  unique_ptr<Map> map{builder.Build()};

  auto point_ptr1 = map->get_point(point1);
  ASSERT_TRUE(point_ptr1);
  ASSERT_EQ(point_ptr1->get_in_paths().size(), 0);
}

TEST_F(MapBuilderTest, AddLocation) {
  auto point0 = builder.AddPoint({0, 0, 0});
  auto point1 = builder.AddPoint({0, 0, 0});
  auto path2 = builder.AddPath(point0, point1, 1);

  auto location3 = builder.AddLocation({"Load"}, {point0});
  builder.Link(point1, location3);

  unique_ptr<Map> map{builder.Build()};

  auto location_ptr3 = map->get_location(location3);
  ASSERT_TRUE(location_ptr3);
  ASSERT_EQ(location_ptr3->get_linked_points().size(), 2);
}

TEST_F(MapBuilderTest, UnlinkLocation) {
  auto point0 = builder.AddPoint({0, 0, 0});
  auto point1 = builder.AddPoint({0, 0, 0});
  auto path2 = builder.AddPath(point0, point1, 1);

  auto location3 = builder.AddLocation({"Load"}, {point0});
  builder.Link(point1, location3);
  builder.Unlink(point0, location3);

  unique_ptr<Map> map{builder.Build()};

  auto location_ptr3 = map->get_location(location3);
  auto point_ptr0 = map->get_point(point0);
  auto point_ptr1 = map->get_point(point1);
  ASSERT_TRUE(location_ptr3);
  ASSERT_TRUE(point_ptr0);
  ASSERT_TRUE(point_ptr1);
  ASSERT_EQ(location_ptr3->get_linked_points().size(), 1);
  ASSERT_FALSE(point_ptr0->get_linked_location().has_value());
  ASSERT_TRUE(point_ptr1->get_linked_location().has_value());
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

  auto block_ptr5 = map->get_block(block5);
  auto block_ptr6 = map->get_block(block6);
  ASSERT_TRUE(block_ptr5);
  ASSERT_TRUE(block_ptr6);
  ASSERT_NE(block_ptr5->get_resources().find(point0),
            block_ptr5->get_resources().end());
  ASSERT_EQ(block_ptr6->get_resources().find(point0),
            block_ptr6->get_resources().end());
  ASSERT_EQ(block_ptr5->get_resources().size(), 2);
  ASSERT_EQ(block_ptr6->get_resources().size(), 2);
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

  auto block_ptr5 = map->get_block(block5);
  auto block_ptr6 = map->get_block(block6);
  ASSERT_TRUE(block_ptr5);
  ASSERT_FALSE(block_ptr6);
  ASSERT_EQ(block_ptr5->get_resources().find(point0),
            block_ptr5->get_resources().end());
  ASSERT_EQ(block_ptr5->get_resources().size(), 1);
}