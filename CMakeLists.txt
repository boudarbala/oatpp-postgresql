cmake_minimum_required(VERSION 3.20)

###################################################################################################
## These variables are passed to oatpp-module-install.cmake script
## use these variables to configure module installation

set(OATPP_THIS_MODULE_NAME oatpp-sqlserver) ## name of the module (also name of folders in installation dirs)
set(OATPP_THIS_MODULE_VERSION "1.4.0") ## version of the module (also sufix of folders in installation dirs)
set(OATPP_THIS_MODULE_LIBRARIES oatpp-sqlserver) ## list of libraries to find when find_package is called
set(OATPP_THIS_MODULE_TARGETS oatpp-sqlserver) ## list of targets to install
set(OATPP_THIS_MODULE_DIRECTORIES oatpp-sqlserver) ## list of directories to install

###################################################################################################

project(${OATPP_THIS_MODULE_NAME}
        VERSION ${OATPP_THIS_MODULE_VERSION}
        LANGUAGES CXX
        ## HOMEPAGE_URL "https://github.com/oatpp/oatpp-sqlserver"
        ## DESCRIPTION "Something about sqlserver"
)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(OATPP_DIR_SRC "Path to oatpp module directory (sources)")
option(OATPP_DIR_LIB "Path to directory with liboatpp (directory containing ex: liboatpp.so or liboatpp.dynlib)")
option(OATPP_BUILD_TESTS "Build tests for this module" ON)
option(OATPP_INSTALL "Install module binaries" ON)

set(OATPP_MODULES_LOCATION "SUBMODULE" CACHE STRING "Location where to find oatpp modules. can be [INSTALLED|EXTERNAL|CUSTOM|SUBMODULE]")

###################################################################################################
## get oatpp main module in specified location

set(OATPP_MODULES_LOCATION_INSTALLED INSTALLED)
set(OATPP_MODULES_LOCATION_EXTERNAL EXTERNAL)
set(OATPP_MODULES_LOCATION_CUSTOM CUSTOM)
set(OATPP_MODULES_LOCATION_SUBMODULE SUBMODULE)

if(OATPP_MODULES_LOCATION STREQUAL OATPP_MODULES_LOCATION_INSTALLED)

    message("Finding oatpp in location=INSTALLED")

    find_package(oatpp ${OATPP_THIS_MODULE_VERSION} REQUIRED)

    get_target_property(OATPP_INCLUDE oatpp::oatpp INTERFACE_INCLUDE_DIRECTORIES)
    message("OATPP_INCLUDE=${OATPP_INCLUDE}")

    get_target_property(OATPP_TEST_INCLUDE oatpp::oatpp-test INTERFACE_INCLUDE_DIRECTORIES)
    message("OATPP_TEST_INCLUDE=${OATPP_TEST_INCLUDE}")

elseif(OATPP_MODULES_LOCATION STREQUAL OATPP_MODULES_LOCATION_EXTERNAL)

    message("Finding oatpp in location=EXTERNAL")

    include(ExternalProject)

    set(MODULE_WAIT_DEPS ON)

    set(LIB_OATPP_EXTERNAL "lib_oatpp_external")
    ExternalProject_Add(${LIB_OATPP_EXTERNAL}
            GIT_REPOSITORY "https://github.com/oatpp/oatpp.git"
            GIT_TAG origin/master
            CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DOATPP_INSTALL=OFF -DOATPP_BUILD_TESTS=OFF
            INSTALL_COMMAND cmake -E echo "SKIP INSTALL '${LIB_OATPP_EXTERNAL}'"
    )

    ExternalProject_Get_Property(${LIB_OATPP_EXTERNAL} BINARY_DIR)
    set(OATPP_DIR_LIB ${BINARY_DIR}/src)

    ExternalProject_Get_Property(${LIB_OATPP_EXTERNAL} SOURCE_DIR)
    set(OATPP_DIR_SRC ${SOURCE_DIR}/src)

    message("OATPP_DIR_SRC --> '${OATPP_DIR_SRC}'")
    message("OATPP_DIR_LIB --> '${OATPP_DIR_LIB}'")

elseif(OATPP_MODULES_LOCATION STREQUAL OATPP_MODULES_LOCATION_SUBMODULE)

    message("Finding oatpp in location=SUBMODULE")

    # Add oatpp submodule as subdirectory
    set(OATPP_BUILD_TESTS OFF CACHE BOOL "Don't build oatpp tests")
    add_subdirectory(deps/oatpp)
    
    # Set paths for oatpp
    set(OATPP_DIR_SRC ${CMAKE_CURRENT_SOURCE_DIR}/deps/oatpp/src)
    set(OATPP_DIR_LIB ${CMAKE_CURRENT_BINARY_DIR}/deps/oatpp/src)

    message("OATPP_DIR_SRC --> '${OATPP_DIR_SRC}'")
    message("OATPP_DIR_LIB --> '${OATPP_DIR_LIB}'")

elseif(OATPP_MODULES_LOCATION STREQUAL OATPP_MODULES_LOCATION_CUSTOM)

    message("Finding oatpp in location=CUSTOM")

    message("OATPP_DIR_SRC --> '${OATPP_DIR_SRC}'")
    message("OATPP_DIR_LIB --> '${OATPP_DIR_LIB}'")

else()
    message("FATAL_ERROR Unknown location to find oatpp '${OATPP_MODULES_LOCATION}'")
endif()

if(OATPP_DIR_LIB)
    link_directories(${OATPP_DIR_LIB})
endif()

###################################################################################################
## get dependencies

message("\n############################################################################")
message("## ${OATPP_THIS_MODULE_NAME} module. Resolving dependencies...\n")

# Find ODBC - Cross-platform detection
if(WIN32)
    # Windows ODBC detection
    find_path(ODBC_INCLUDE_DIR 
        NAMES sql.h
        PATHS
            "$ENV{ProgramFiles}/Microsoft SQL Server/Client SDK/ODBC/130/SDK/Include"
            "$ENV{ProgramFiles}/Microsoft SQL Server/Client SDK/ODBC/110/SDK/Include"
            "$ENV{ProgramFiles}/Microsoft SQL Server/Client SDK/ODBC/100/SDK/Include"
            "C:/Program Files/Microsoft SQL Server/Client SDK/ODBC/130/SDK/Include"
            "C:/Program Files/Microsoft SQL Server/Client SDK/ODBC/110/SDK/Include"
            "C:/Program Files/Microsoft SQL Server/Client SDK/ODBC/100/SDK/Include"
            "${CMAKE_CURRENT_SOURCE_DIR}/deps/windows/include"
    )
    
    find_library(ODBC_LIBRARY 
        NAMES odbc32 odbccp32
        PATHS
            "$ENV{ProgramFiles}/Microsoft SQL Server/Client SDK/ODBC/130/SDK/Lib/x64"
            "$ENV{ProgramFiles}/Microsoft SQL Server/Client SDK/ODBC/110/SDK/Lib/x64"
            "$ENV{ProgramFiles}/Microsoft SQL Server/Client SDK/ODBC/100/SDK/Lib/x64"
            "C:/Program Files/Microsoft SQL Server/Client SDK/ODBC/130/SDK/Lib/x64"
            "C:/Program Files/Microsoft SQL Server/Client SDK/ODBC/110/SDK/Lib/x64"
            "C:/Program Files/Microsoft SQL Server/Client SDK/ODBC/100/SDK/Lib/x64"
            "${CMAKE_CURRENT_SOURCE_DIR}/deps/windows/lib"
    )
    
    # If not found, try system directories
    if(NOT ODBC_LIBRARY)
        find_library(ODBC_LIBRARY NAMES odbc32)
    endif()
else()
    # Unix/Linux ODBC detection (existing logic)
    find_path(ODBC_INCLUDE_DIR sql.h PATHS /usr/include /usr/local/include)
    find_library(ODBC_LIBRARY NAMES odbc PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu)
endif()

if(NOT ODBC_INCLUDE_DIR OR NOT ODBC_LIBRARY)
    if(WIN32)
        message(FATAL_ERROR "ODBC library not found. Please install Microsoft ODBC Driver for SQL Server or Visual Studio with C++ tools.")
    else()
        message(FATAL_ERROR "ODBC library not found. Please install unixodbc-dev or similar package.")
    endif()
endif()

message("ODBC_INCLUDE_DIR=${ODBC_INCLUDE_DIR}")
message("ODBC_LIBRARY=${ODBC_LIBRARY}")

message("\n############################################################################\n")

###################################################################################################
## define targets

include(cmake/module-utils.cmake)

add_subdirectory("src")

if(OATPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory("test")
endif()

# Optional: build examples
option(OATPP_BUILD_EXAMPLES "Build examples for this module" ON)
if(OATPP_BUILD_EXAMPLES)
    add_subdirectory("examples/simple")
endif()
