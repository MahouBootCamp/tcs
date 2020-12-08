cmake_minimum_required(VERSION 3.0.0)
project(libtcs VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)
find_package(Boost COMPONENTS graph log)

include(CTest)
enable_testing()

add_library(libtcs 
    src/kernal.cc
    src/map_builder.cc
    src/map.cc
    src/order_pool.cc
    src/router.cc
    src/shortest_algorithm.cc
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
