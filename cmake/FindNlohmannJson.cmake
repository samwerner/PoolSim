include(ExternalProject)

ExternalProject_Add(
    nlohmann-json
    URL https://github.com/nlohmann/json/releases/download/v3.5.0/json.hpp
    PREFIX "${CMAKE_BINARY_DIR}/external"
    DOWNLOAD_DIR "${CMAKE_BINARY_DIR}/external/include/nlohmann"
    DOWNLOAD_NO_EXTRACT 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND "")
