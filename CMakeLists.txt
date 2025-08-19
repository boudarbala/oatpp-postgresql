cmake_minimum_required(VERSION 3.20)

project(oatpp-sqlserver
        VERSION "1.4.0"
        LANGUAGES CXX
)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(OATPP_BUILD_TESTS "Build tests for this module" ON)

# Find required packages
find_package(oatpp REQUIRED)

# Find ODBC for SQL Server
find_path(ODBC_INCLUDE_DIR sql.h PATHS /usr/include /usr/local/include)
find_library(ODBC_LIBRARY NAMES odbc PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu)

if(NOT ODBC_INCLUDE_DIR OR NOT ODBC_LIBRARY)
    message(FATAL_ERROR "ODBC library not found. Please install unixodbc-dev or similar package.")
endif()

# Define targets
add_subdirectory("src")

if(OATPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory("test")
endif()
