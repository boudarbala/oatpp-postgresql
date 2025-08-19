#include "oatpp/Environment.hpp"
#include "oatpp/Types.hpp"
#include "oatpp-sqlserver/ConnectionProvider.hpp"
#include "oatpp-sqlserver/Executor.hpp"
#include "oatpp/macro/codegen.hpp"

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
    if(handle) {
      std::cout << "[OK] Connected" << std::endl;
    } else {
      std::cout << "[FAIL] No handle" << std::endl; return 1;
    }

    // 2) ORM DbClient demonstration
    auto executor = std::make_shared<oatpp::sqlserver::Executor>(connectionProvider);

    #include OATPP_CODEGEN_BEGIN(DbClient)

    class MyClient : public oatpp::orm::DbClient {
    public:
      MyClient(const std::shared_ptr<oatpp::orm::Executor>& exec)
        : oatpp::orm::DbClient(exec) {}

      QUERY(dropIfExists, "IF OBJECT_ID('dbo.TestUsers','U') IS NOT NULL DROP TABLE dbo.TestUsers;", PREPARE(false))
      QUERY(createTable,  "CREATE TABLE dbo.TestUsers (id INT IDENTITY(1,1) PRIMARY KEY, name NVARCHAR(100) NOT NULL, email NVARCHAR(255) UNIQUE NOT NULL, isActive BIT DEFAULT 1);", PREPARE(false))
      QUERY(insert1,      "INSERT INTO dbo.TestUsers (name, email, isActive) VALUES ('John Doe','john.doe@example.com',1);", PREPARE(false))
      QUERY(insert2,      "INSERT INTO dbo.TestUsers (name, email, isActive) VALUES ('Jane Smith','jane.smith@example.com',1);", PREPARE(false))
      QUERY(insert3,      "INSERT INTO dbo.TestUsers (name, email, isActive) VALUES ('Bob Johnson','bob.johnson@example.com',0);", PREPARE(false))
      QUERY(updateRow,    "UPDATE dbo.TestUsers SET name='John Doe Updated' WHERE id=1;", PREPARE(false))
      QUERY(dropFinal,    "DROP TABLE dbo.TestUsers;", PREPARE(false))
    };

    #include OATPP_CODEGEN_END(DbClient)

    MyClient client(executor);

    std::cout << "\n2. Creating test table via ORM..." << std::endl;
    (void)client.dropIfExists();
    auto createRes = client.createTable();
    if(!createRes->isSuccess()) { std::cout << "[FAIL] " << createRes->getErrorMessage()->c_str() << std::endl; return 1; }
    std::cout << "[OK] Table created" << std::endl;

    std::cout << "\n3. Inserting sample data via ORM..." << std::endl;
    (void)client.insert1();
    (void)client.insert2();
    (void)client.insert3();
    std::cout << "[OK] Inserts done" << std::endl;

    std::cout << "\n4. Updating user via ORM..." << std::endl;
    auto upd = client.updateRow();
    if(!upd->isSuccess()) { std::cout << "[FAIL] " << upd->getErrorMessage()->c_str() << std::endl; }
    else { std::cout << "[OK] Update done" << std::endl; }

    std::cout << "\n5. Cleaning up via ORM..." << std::endl;
    auto drop = client.dropFinal();
    if(!drop->isSuccess()) { std::cout << "[FAIL] " << drop->getErrorMessage()->c_str() << std::endl; }
    else { std::cout << "[OK] Table dropped" << std::endl; }

    std::cout << "\n=== Example completed successfully! ===" << std::endl;

  } catch (const std::exception& e) {
    std::cerr << "\n✗ Error: " << e.what() << std::endl;
    oatpp::Environment::destroy();
    return 1;
  }

  oatpp::Environment::destroy();
  return 0;
}
