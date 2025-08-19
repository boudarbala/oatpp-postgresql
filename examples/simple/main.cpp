#include "oatpp/Environment.hpp"
#include "oatpp/Types.hpp"
#include "oatpp-sqlserver/ConnectionProvider.hpp"

// ODBC headers
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <sql.h>
#include <sqlext.h>

#include <iostream>

int main() {
  oatpp::Environment::init();

  // Connection string (adjust Driver version if necessary)
  const auto connectionString = oatpp::String(R"(Driver={ODBC Driver 18 for SQL Server};Server=BOUDARBALA\SQLEXPRESS;Database=dbtest;Trusted_Connection=Yes;Encrypt=No;TrustServerCertificate=Yes;)");

  std::cout << "=== Oat++ SQL Server Example ===" << std::endl;
  std::cout << "Connection String: " << connectionString->c_str() << std::endl;

  try {
    // Create connection provider
    auto connectionProvider = std::make_shared<oatpp::sqlserver::ConnectionProvider>(connectionString);

    // 1) Connectivity
    std::cout << "\n1. Testing basic connectivity..." << std::endl;
    auto handle = connectionProvider->get();
    if(handle) { std::cout << "[OK] Connected" << std::endl; } else { std::cout << "[FAIL] No handle" << std::endl; return 1; }

    // Use raw ODBC to execute SQL (Executor is not fully implemented yet)
    HDBC hdbc = handle.object->getHandle();
    auto execDirect = [&](const char* sql) -> bool {
      SQLHSTMT hstmt = SQL_NULL_HSTMT;
      if(SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt) != SQL_SUCCESS) {
        std::cout << "[FAIL] SQLAllocHandle(STMT)" << std::endl;
        return false;
      }
      auto rc = SQLExecDirect(hstmt, (SQLCHAR*)sql, SQL_NTS);
      if(rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
        SQLCHAR state[6] = {0}; SQLINTEGER native = 0; SQLCHAR msg[512] = {0}; SQLSMALLINT len = 0;
        SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, state, &native, msg, sizeof msg, &len);
        std::cout << "[FAIL] SQLExecDirect: " << (const char*)msg << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        return false;
      }
      SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
      return true;
    };

    // 2) DDL
    std::cout << "\n2. Creating test table..." << std::endl;
    (void)execDirect("IF OBJECT_ID('dbo.TestUsers','U') IS NOT NULL DROP TABLE dbo.TestUsers;");
    if(!execDirect("CREATE TABLE dbo.TestUsers (id INT IDENTITY(1,1) PRIMARY KEY, name NVARCHAR(100) NOT NULL, email NVARCHAR(255) UNIQUE NOT NULL, isActive BIT DEFAULT 1);") ) return 1;
    std::cout << "[OK] Table created" << std::endl;

    // 3) Inserts
    std::cout << "\n3. Inserting sample data..." << std::endl;
    execDirect("INSERT INTO dbo.TestUsers (name, email, isActive) VALUES ('John Doe', 'john.doe@example.com', 1);");
    execDirect("INSERT INTO dbo.TestUsers (name, email, isActive) VALUES ('Jane Smith', 'jane.smith@example.com', 1);");
    execDirect("INSERT INTO dbo.TestUsers (name, email, isActive) VALUES ('Bob Johnson', 'bob.johnson@example.com', 0);");
    std::cout << "[OK] Inserts done" << std::endl;

    // 4) Update
    std::cout << "\n4. Updating user..." << std::endl;
    if(execDirect("UPDATE dbo.TestUsers SET name='John Doe Updated' WHERE id=1;"))
      std::cout << "[OK] Update done" << std::endl;

    // 5) Cleanup
    std::cout << "\n5. Cleaning up..." << std::endl;
    if(execDirect("DROP TABLE dbo.TestUsers;"))
      std::cout << "[OK] Table dropped" << std::endl;

    std::cout << "\n=== Example completed successfully! ===" << std::endl;

  } catch (const std::exception& e) {
    std::cerr << "\n✗ Error: " << e.what() << std::endl;
    oatpp::Environment::destroy();
    return 1;
  }

  oatpp::Environment::destroy();
  return 0;
}
