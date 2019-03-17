add_executable(poolsim-bin main.cpp)

target_link_libraries(poolsim-bin PRIVATE poolsim)

add_dependencies(poolsim-bin poolsim)

target_include_directories(poolsim-bin PRIVATE ../libpoolsim)

set_target_properties(poolsim-bin PROPERTIES OUTPUT_NAME poolsim)

install(TARGETS poolsim-bin DESTINATION bin)

