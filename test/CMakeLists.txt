add_executable(module-tests
        oatpp-postgresql/tests.cpp
        )

set_target_properties(module-tests PROPERTIES
        CXX_STANDARD 11
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
)

target_include_directories(module-tests
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(module-tests
        PRIVATE oatpp::oatpp
        PRIVATE oatpp::oatpp-test
        PRIVATE oatpp-sqlserver
)

add_test(module-tests module-tests)
