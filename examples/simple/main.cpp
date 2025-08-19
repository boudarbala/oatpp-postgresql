#include "oatpp/Environment.hpp"
#include "oatpp/Types.hpp"
#include "oatpp-sqlserver/ConnectionProvider.hpp"
#include "oatpp-sqlserver/Executor.hpp"
#include "oatpp/data/share/StringTemplate.hpp"

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

    // Use ORM executor (now backed by minimal ODBC calls)
    auto executor = std::make_shared<oatpp::sqlserver::Executor>(connectionProvider);

    // 2) DDL
    std::cout << "\n2. Creating test table..." << std::endl;
  auto dropTpl = executor->parseQueryTemplate("drop", oatpp::String("IF OBJECT_ID('dbo.TestUsers','U') IS NOT NULL DROP TABLE dbo.TestUsers;"), {}, false);
  (void)executor->execute(dropTpl, {}, executor->createTypeResolver(), nullptr);
  auto createTpl = executor->parseQueryTemplate("create", oatpp::String("CREATE TABLE dbo.TestUsers (id INT IDENTITY(1,1) PRIMARY KEY, name NVARCHAR(100) NOT NULL, email NVARCHAR(255) UNIQUE NOT NULL, isActive BIT DEFAULT 1);"), {}, false);
  auto createRes = executor->execute(createTpl, {}, executor->createTypeResolver(), nullptr);
    if(!createRes->isSuccess()) { std::cout << "[FAIL] " << createRes->getErrorMessage()->c_str() << std::endl; return 1; }
    std::cout << "[OK] Table created" << std::endl;

    // 3) Inserts
    std::cout << "\n3. Inserting sample data..." << std::endl;
  auto ins1 = executor->parseQueryTemplate("ins1", oatpp::String("INSERT INTO dbo.TestUsers (name, email, isActive) VALUES ('John Doe', 'john.doe@example.com', 1);"), {}, false);
  (void)executor->execute(ins1, {}, executor->createTypeResolver(), nullptr);
  auto ins2 = executor->parseQueryTemplate("ins2", oatpp::String("INSERT INTO dbo.TestUsers (name, email, isActive) VALUES ('Jane Smith', 'jane.smith@example.com', 1);"), {}, false);
  (void)executor->execute(ins2, {}, executor->createTypeResolver(), nullptr);
  auto ins3 = executor->parseQueryTemplate("ins3", oatpp::String("INSERT INTO dbo.TestUsers (name, email, isActive) VALUES ('Bob Johnson', 'bob.johnson@example.com', 0);"), {}, false);
  (void)executor->execute(ins3, {}, executor->createTypeResolver(), nullptr);
    std::cout << "[OK] Inserts done" << std::endl;

    // 4) Update
    std::cout << "\n4. Updating user..." << std::endl;
  auto updTpl = executor->parseQueryTemplate("upd", oatpp::String("UPDATE dbo.TestUsers SET name='John Doe Updated' WHERE id=1;"), {}, false);
  auto upd = executor->execute(updTpl, {}, executor->createTypeResolver(), nullptr);
    if(!upd->isSuccess()) { std::cout << "[FAIL] " << upd->getErrorMessage()->c_str() << std::endl; }
    else { std::cout << "[OK] Update done" << std::endl; }

    // 5) Cleanup
    std::cout << "\n5. Cleaning up..." << std::endl;
  auto dropFinalTpl = executor->parseQueryTemplate("dropFinal", oatpp::String("DROP TABLE dbo.TestUsers;"), {}, false);
  auto drop = executor->execute(dropFinalTpl, {}, executor->createTypeResolver(), nullptr);
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
