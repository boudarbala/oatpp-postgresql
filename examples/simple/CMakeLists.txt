cmake_minimum_required(VERSION 3.20)

project(simple-sqlserver LANGUAGES CXX)

add_executable(simple-sqlserver
    main.cpp
)

set_target_properties(simple-sqlserver PROPERTIES
    CXX_STANDARD 11
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
)

# Include module utils (optional) to get target_link_oatpp helper
include(${CMAKE_SOURCE_DIR}/cmake/module-utils.cmake OPTIONAL)

# Link against oatpp and this module
if (COMMAND target_link_oatpp)
    target_link_oatpp(simple-sqlserver)
endif()

target_link_libraries(simple-sqlserver PRIVATE oatpp-sqlserver)

