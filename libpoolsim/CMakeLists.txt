file(GLOB impl_sources "*.cpp")
file(GLOB header_sources "*.h")

find_package(Cli REQUIRED)
find_package(Spdlog REQUIRED)
find_package(NlohmannJson REQUIRED)
find_package(FastCppCsvParser REQUIRED)
find_package(Boost 1.65.0 COMPONENTS iostreams)

add_library(poolsim SHARED ${impl_sources} ${header_sources})

add_dependencies(poolsim spdlog cli11 nlohmann-json fast-cpp-csv-parser)

target_include_directories(poolsim PUBLIC ${CMAKE_BINARY_DIR}/external/include)

set_target_properties(poolsim PROPERTIES PUBLIC_HEADER "${header_sources}")

target_link_libraries(poolsim PUBLIC)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(poolsim ${Boost_LIBRARIES})
    add_definitions(-DUSE_BOOST_IOSTREAMS=1)
endif()

install(
    TARGETS poolsim
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/poolsim
)
install(
    DIRECTORY ${CMAKE_BINARY_DIR}/external/include DESTINATION .
)
