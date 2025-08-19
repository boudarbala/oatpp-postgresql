
add_library(oatpp-sqlserver
        oatpp-sqlserver/mapping/type/Uuid.cpp
        oatpp-sqlserver/mapping/type/Uuid.hpp
        oatpp-sqlserver/mapping/Deserializer.cpp
        oatpp-sqlserver/mapping/Deserializer.hpp
        oatpp-sqlserver/mapping/Oid.hpp
        oatpp-sqlserver/mapping/ResultMapper.cpp
        oatpp-sqlserver/mapping/ResultMapper.hpp
        oatpp-sqlserver/mapping/Serializer.cpp
        oatpp-sqlserver/mapping/Serializer.hpp
        oatpp-sqlserver/Connection.cpp
        oatpp-sqlserver/Connection.hpp
        oatpp-sqlserver/ConnectionProvider.cpp
        oatpp-sqlserver/ConnectionProvider.hpp
        oatpp-sqlserver/Executor.cpp
        oatpp-sqlserver/Executor.hpp
        oatpp-sqlserver/QueryResult.cpp
        oatpp-sqlserver/QueryResult.hpp
        oatpp-sqlserver/Types.hpp
        oatpp-sqlserver/orm.hpp)

set_target_properties(oatpp-sqlserver PROPERTIES
        CXX_STANDARD 11
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
)

target_include_directories(oatpp-sqlserver
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        ${ODBC_INCLUDE_DIR}
)

target_link_libraries(oatpp-sqlserver
        PUBLIC oatpp::oatpp
        PUBLIC ${ODBC_LIBRARY}
)

if(MSVC)
        target_compile_options(oatpp-sqlserver PRIVATE "/bigobj")
endif()
