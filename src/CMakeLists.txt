
add_library(${OATPP_THIS_MODULE_NAME}
        oatpp-sqlserver/mapping/type/Uuid.cpp
        oatpp-sqlserver/mapping/type/Uuid.hpp
        oatpp-sqlserver/mapping/type/Decimal.cpp
        oatpp-sqlserver/mapping/type/Decimal.hpp
        oatpp-sqlserver/mapping/type/DateTime.cpp
        oatpp-sqlserver/mapping/type/DateTime.hpp
        oatpp-sqlserver/mapping/Deserializer.cpp
        oatpp-sqlserver/mapping/Deserializer.hpp
        oatpp-sqlserver/mapping/Oid.hpp
        oatpp-sqlserver/mapping/SqlArray.cpp
        oatpp-sqlserver/mapping/SqlArray.hpp
        oatpp-sqlserver/mapping/ResultMapper.cpp
        oatpp-sqlserver/mapping/ResultMapper.hpp
        oatpp-sqlserver/mapping/Serializer.cpp
        oatpp-sqlserver/mapping/Serializer.hpp
        oatpp-sqlserver/ql_template/Parser.cpp
        oatpp-sqlserver/ql_template/Parser.hpp
        oatpp-sqlserver/ql_template/TemplateValueProvider.cpp
        oatpp-sqlserver/ql_template/TemplateValueProvider.hpp
        oatpp-sqlserver/Connection.cpp
        oatpp-sqlserver/Connection.hpp
        oatpp-sqlserver/ConnectionProvider.cpp
        oatpp-sqlserver/ConnectionProvider.hpp
        oatpp-sqlserver/Executor.cpp
        oatpp-sqlserver/Executor.hpp
        oatpp-sqlserver/QueryResult.cpp
        oatpp-sqlserver/QueryResult.hpp
        oatpp-sqlserver/QueryBuilder.hpp
        oatpp-sqlserver/AdvancedDbClient.hpp
        oatpp-sqlserver/Types.hpp
        oatpp-sqlserver/orm.hpp)

set_target_properties(${OATPP_THIS_MODULE_NAME} PROPERTIES
        CXX_STANDARD 11
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
)

if(OATPP_MODULES_LOCATION STREQUAL OATPP_MODULES_LOCATION_EXTERNAL)
    add_dependencies(${OATPP_THIS_MODULE_NAME} ${LIB_OATPP_EXTERNAL})
endif()

target_link_oatpp(${OATPP_THIS_MODULE_NAME})

target_include_directories(${OATPP_THIS_MODULE_NAME}
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        ${ODBC_INCLUDE_DIR}
)

target_link_libraries(${OATPP_THIS_MODULE_NAME}
        PUBLIC ${ODBC_LIBRARY}
)

if(MSVC)
        target_compile_options(${OATPP_THIS_MODULE_NAME} PRIVATE "/bigobj")
endif()

#######################################################################################################
## install targets

if(OATPP_INSTALL)
    include("../cmake/module-install.cmake")
endif()
