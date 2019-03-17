include(ExternalProject)

ExternalProject_Add(
    cli11
    URL https://github.com/CLIUtils/CLI11/releases/download/v1.7.1/CLI11.hpp
    PREFIX "${CMAKE_BINARY_DIR}/external"
    DOWNLOAD_DIR "${CMAKE_BINARY_DIR}/external/include"
    DOWNLOAD_NO_EXTRACT 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND "")

