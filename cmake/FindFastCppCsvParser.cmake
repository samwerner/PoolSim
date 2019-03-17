include(ExternalProject)

ExternalProject_Add(
    fast-cpp-csv-parser
    URL https://raw.githubusercontent.com/ben-strasser/fast-cpp-csv-parser/5a3443e05b5f2993c903ae2881a95390265bb54b/csv.h
    PREFIX "${CMAKE_BINARY_DIR}/external"
    DOWNLOAD_DIR "${CMAKE_BINARY_DIR}/external/include"
    DOWNLOAD_NO_EXTRACT 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND "")

