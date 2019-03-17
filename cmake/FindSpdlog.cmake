include(ExternalProject)

ExternalProject_Add(
    spdlog
    URL https://github.com/gabime/spdlog/archive/v1.3.1.tar.gz
    PREFIX "${CMAKE_BINARY_DIR}/external"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND mkdir -p ${CMAKE_BINARY_DIR}/external/include && cp -r <SOURCE_DIR>/include/spdlog ${CMAKE_BINARY_DIR}/external/include
)
