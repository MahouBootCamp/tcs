cmake_minimum_required(VERSION 3.0.0)
project(tcs_test VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

add_subdirectory(libtcs)
add_subdirectory(googletest)

add_executable(tcs_unit_test
    src/map_test.cc
)
target_link_libraries(tcs_unit_test 
    PUBLIC libtcs
    PUBLIC GTest::GTest 
    PUBLIC GTest::Main
)
target_include_directories(tcs_unit_test 
    PUBLIC ${libtcs_SOURCE_DIR}/include
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

add_executable(tcs_test main.cc)
target_link_libraries(tcs_test 
    PUBLIC libtcs
)
target_include_directories(tcs_test 
    PUBLIC ${libtcs_SOURCE_DIR}/include
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_test(test_all tcs_test)
