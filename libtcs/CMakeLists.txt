cmake_minimum_required(VERSION 3.0.0)
project(libtcs VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

set(Boost_USE_STATIC_LIBS ON)

find_package(Boost COMPONENTS graph log)
find_package(Threads REQUIRED)

include(CTest)
enable_testing()

add_library(libtcs 
    src/controller_pool.cc
    src/default_controller.cc
    src/default_dispatcher.cc
    src/default_router.cc
    src/default_scheduler.cc
    src/kernel.cc
    src/map_builder.cc
    src/map_service.cc
    src/map.cc
    src/order_pool.cc
    src/phase0_check_new_order.cc
    src/phase1_finish_withdrawal.cc
    src/phase2_assign_next_drive_order.cc
    src/phase3_assign_reserved_order.cc
    src/phase4_assign_free_order.cc
    src/phase5_charge_vehicle.cc
    src/phase6_park_vehicle.cc
    src/reservation_pool.cc
    src/reserve_order_pool.cc
    src/shortest_path_algorithm.cc
    src/sim_vehicle_adapter.cc
    src/transport_order_service.cc
    src/universal_dispatch_util.cc
    src/vehicle_service.cc
)

target_include_directories(libtcs
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    PUBLIC ${Boost_INCLUDE_DIRS}
)

target_link_libraries(libtcs 
    PUBLIC Threads::Threads
    PUBLIC ${Boost_LIBRARIES}
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
