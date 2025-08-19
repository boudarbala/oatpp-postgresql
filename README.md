# oatpp-sqlserver

Oat++ ORM adapter for SQL Server.

## Build

Prerequisites:
- oatpp framework
- SQL Server ODBC driver (unixodbc-dev)

```bash
mkdir build && cd build
cmake ..
make
```
## Usage

```cpp
#include "oatpp-sqlserver/orm.hpp"

// Create connection provider
auto connectionProvider = std::make_shared<oatpp::sqlserver::ConnectionProvider>("<connection-string>");

// Create connection pool  
auto connectionPool = oatpp::sqlserver::ConnectionPool::createShared(
    connectionProvider, 
    10, // max connections
    std::chrono::seconds(5) // connection TTL
);

// Create executor
auto executor = std::make_shared<oatpp::sqlserver::Executor>(connectionPool);
```

## Supported Data Types

- SMALLINT, INT, BIGINT
- DATETIME  
- VARCHAR, NVARCHAR
- REAL, FLOAT
- BIT
- UNIQUEIDENTIFIER
