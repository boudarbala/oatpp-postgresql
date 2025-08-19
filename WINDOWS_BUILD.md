# Windows Build Instructions

This oatpp-sqlserver module now supports building on Windows with ODBC.

## Windows Prerequisites

1. **Visual Studio with C++ support** or **Visual Studio Build Tools**
2. **Microsoft ODBC Driver for SQL Server** or **SQL Server Native Client**
3. **CMake 3.20 or later**
4. **Git** for submodule management

## Building on Windows

```cmd
git clone https://github.com/boudarbala/oatpp-postgresql.git
cd oatpp-postgresql
git submodule update --init --recursive
mkdir build
cd build
cmake -DOATPP_MODULES_LOCATION=SUBMODULE -DOATPP_BUILD_TESTS=OFF ..
cmake --build . --config Release
```

## Building on Unix/Linux

```bash
git clone https://github.com/boudarbala/oatpp-postgresql.git
cd oatpp-postgresql
git submodule update --init --recursive
sudo apt-get install unixodbc-dev  # Ubuntu/Debian
mkdir build
cd build
cmake -DOATPP_MODULES_LOCATION=SUBMODULE -DOATPP_BUILD_TESTS=OFF ..
make -j$(nproc)
```

## Key Changes Made

1. **Cross-Platform ODBC Detection**: The CMakeLists.txt now detects ODBC libraries on both Windows and Unix systems
2. **Submodule Dependencies**: oatpp is now included as a git submodule instead of being downloaded during build
3. **Namespace Consistency**: All code now uses `oatpp::sqlserver` namespace consistently
4. **Windows Header Compatibility**: Fixed BOOL macro conflicts between Windows ODBC headers and oatpp

## Current Status

- ✅ **Core Library**: Builds successfully on Linux and should build on Windows
- ✅ **Dependency Management**: Uses git submodules for reproducible builds  
- ⚠️ **Database Implementation**: Core executor functions are stubbed and need ODBC implementation
- ❌ **Tests**: Disabled during conversion, need to be rewritten for SQL Server

## Notes for Further Development

The core database operations (query execution, parameter binding, result processing) are currently stubbed with TODO comments. These need to be implemented using ODBC API functions like:

- `SQLPrepare` / `SQLExecute` for prepared statements
- `SQLExecDirect` for direct query execution  
- `SQLBindParameter` for parameter binding
- `SQLFetch` / `SQLGetData` for result retrieval
- `SQLDescribeCol` for metadata

This provides a solid foundation for Windows compatibility and dependency management.