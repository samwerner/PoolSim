add_executable(poolsim-test poolsim_test.cpp)

find_package(GTest REQUIRED)
find_package(GMock REQUIRED)
find_package(Threads REQUIRED)

file(COPY fixtures DESTINATION .)

include_directories(${GTEST_INCLUDE_DIRS})

target_include_directories(poolsim-test PRIVATE ../libpoolsim ${CMAKE_BINARY_DIR}/external/include)

target_link_libraries(poolsim-test PRIVATE poolsim ${CMAKE_THREAD_LIBS_INIT} ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES})

gtest_discover_tests(poolsim-test)

add_test(NAME poolsim-test COMMAND poolsim-test WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
