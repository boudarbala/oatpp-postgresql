#include "oatpp/Environment.hpp"
#include "oatpp-sqlserver/ConnectionProvider.hpp"
#include "oatpp-sqlserver/Executor.hpp"
#include "oatpp/Types.hpp"
#include "oatpp/data/share/StringTemplate.hpp"

#include <iostream>

int main() {
  oatpp::Environment::init();

  // Connection string examples for different scenarios:
  // 1. SQL Server Express with Windows Authentication (as in original)
  // const auto connectionString = oatpp::String(R"(Driver={ODBC Driver 18 for SQL Server};Server=BOUDARBALA\SQLEXPRESS;Trusted_Connection=Yes;Encrypt=No;TrustServerCertificate=Yes;)");
  
  // 2. SQL Server with SQL Authentication
  // const auto connectionString = oatpp::String(R"(Driver={ODBC Driver 18 for SQL Server};Server=localhost;Database=testdb;UID=sa;PWD=YourPassword123;Encrypt=No;TrustServerCertificate=Yes;)");
  
  // 3. Azure SQL Database
  // const auto connectionString = oatpp::String(R"(Driver={ODBC Driver 18 for SQL Server};Server=yourserver.database.windows.net;Database=yourdatabase;UID=yourusername;PWD=yourpassword;Encrypt=Yes;)");

  // Default to the original connection string
  const auto connectionString = oatpp::String(R"(Driver={ODBC Driver 18 for SQL Server};Server=BOUDARBALA\SQLEXPRESS;Trusted_Connection=Yes;Encrypt=No;TrustServerCertificate=Yes;)");

  std::cout << "=== Oat++ SQL Server Example ===" << std::endl;
  std::cout << "Connection String: " << connectionString->c_str() << std::endl;

  try {
    // Create connection provider
    auto connectionProvider = std::make_shared<oatpp::sqlserver::ConnectionProvider>(connectionString);
    
    // Test basic connectivity
    std::cout << "\n1. Testing basic connectivity..." << std::endl;
    auto handle = connectionProvider->get();
    if(handle) {
      std::cout << "✓ Connected to SQL Server successfully." << std::endl;
    } else {
      std::cout << "✗ Failed to get connection handle." << std::endl;
      return 1;
    }

    // Create executor for direct SQL execution
    std::cout << "\n2. Setting up SQL executor..." << std::endl;
    auto executor = std::make_shared<oatpp::sqlserver::Executor>(connectionProvider);
    std::cout << "✓ SQL executor created." << std::endl;

    // Create a test table
    std::cout << "\n3. Creating test table..." << std::endl;
    auto createTableQuery = oatpp::String(
      "CREATE TABLE TestUsers ("
      "id INT IDENTITY(1,1) PRIMARY KEY, "
      "name NVARCHAR(100) NOT NULL, "
      "email NVARCHAR(255) UNIQUE NOT NULL, "
      "isActive BIT DEFAULT 1"
      ");"
    );
    
    try {
      // Drop table if it exists
      auto dropResult = executor->execute(oatpp::data::share::StringTemplate(oatpp::String("DROP TABLE IF EXISTS TestUsers"), {}), {}, executor->createTypeResolver(), nullptr);
      if(dropResult->isSuccess()) {
        std::cout << "✓ Dropped existing TestUsers table (if any)." << std::endl;
      }
    } catch (const std::exception& e) {
      std::cout << "ℹ No existing table to drop (expected): " << e.what() << std::endl;
    }

    auto createResult = executor->execute(oatpp::data::share::StringTemplate(createTableQuery, {}), {}, executor->createTypeResolver(), nullptr);
    if(createResult->isSuccess()) {
      std::cout << "✓ TestUsers table created successfully." << std::endl;
    } else {
      std::cout << "✗ Failed to create TestUsers table: " << createResult->getErrorMessage()->c_str() << std::endl;
      return 1;
    }

    // Insert sample data
    std::cout << "\n4. Inserting sample data..." << std::endl;
    
    auto insertQuery1 = oatpp::String("INSERT INTO TestUsers (name, email, isActive) VALUES ('John Doe', 'john.doe@example.com', 1);");
    auto insertResult1 = executor->execute(oatpp::data::share::StringTemplate(insertQuery1, {}), {}, executor->createTypeResolver(), nullptr);
    if(insertResult1->isSuccess()) {
      std::cout << "✓ Inserted user: John Doe" << std::endl;
    } else {
      std::cout << "✗ Failed to insert John Doe: " << insertResult1->getErrorMessage()->c_str() << std::endl;
    }

    auto insertQuery2 = oatpp::String("INSERT INTO TestUsers (name, email, isActive) VALUES ('Jane Smith', 'jane.smith@example.com', 1);");
    auto insertResult2 = executor->execute(oatpp::data::share::StringTemplate(insertQuery2, {}), {}, executor->createTypeResolver(), nullptr);
    if(insertResult2->isSuccess()) {
      std::cout << "✓ Inserted user: Jane Smith" << std::endl;
    } else {
      std::cout << "✗ Failed to insert Jane Smith: " << insertResult2->getErrorMessage()->c_str() << std::endl;
    }

    auto insertQuery3 = oatpp::String("INSERT INTO TestUsers (name, email, isActive) VALUES ('Bob Johnson', 'bob.johnson@example.com', 0);");
    auto insertResult3 = executor->execute(oatpp::data::share::StringTemplate(insertQuery3, {}), {}, executor->createTypeResolver(), nullptr);
    if(insertResult3->isSuccess()) {
      std::cout << "✓ Inserted user: Bob Johnson (inactive)" << std::endl;
    } else {
      std::cout << "✗ Failed to insert Bob Johnson: " << insertResult3->getErrorMessage()->c_str() << std::endl;
    }

    // Query data
    std::cout << "\n5. Querying all users..." << std::endl;
    auto selectQuery = oatpp::String("SELECT id, name, email, isActive FROM TestUsers ORDER BY id;");
    auto selectResult = executor->execute(oatpp::data::share::StringTemplate(selectQuery, {}), {}, executor->createTypeResolver(), nullptr);
    if(selectResult->isSuccess()) {
      std::cout << "✓ Query successful. Users in table:" << std::endl;
      std::cout << "  (Note: Advanced result mapping requires full ORM setup)" << std::endl;
      std::cout << "  - Query executed without errors" << std::endl;
      std::cout << "  - Position: " << selectResult->getPosition() << std::endl;
      std::cout << "  - Has more data: " << (selectResult->hasMoreToFetch() ? "Yes" : "No") << std::endl;
    } else {
      std::cout << "✗ Failed to query users: " << selectResult->getErrorMessage()->c_str() << std::endl;
    }

    // Update data
    std::cout << "\n6. Updating user data..." << std::endl;
    auto updateQuery = oatpp::String("UPDATE TestUsers SET name = 'John Doe Updated' WHERE id = 1;");
    auto updateResult = executor->execute(oatpp::data::share::StringTemplate(updateQuery, {}), {}, executor->createTypeResolver(), nullptr);
    if(updateResult->isSuccess()) {
      std::cout << "✓ User updated successfully." << std::endl;
    } else {
      std::cout << "✗ Failed to update user: " << updateResult->getErrorMessage()->c_str() << std::endl;
    }

    // Test transaction support
    std::cout << "\n7. Testing transaction support..." << std::endl;
    auto beginResult = executor->begin();
    if(beginResult->isSuccess()) {
      std::cout << "✓ Transaction started." << std::endl;
      
      auto rollbackResult = executor->rollback(beginResult->getConnection());
      if(rollbackResult->isSuccess()) {
        std::cout << "✓ Transaction rolled back successfully." << std::endl;
      } else {
        std::cout << "✗ Failed to rollback transaction: " << rollbackResult->getErrorMessage()->c_str() << std::endl;
      }
    } else {
      std::cout << "✗ Failed to start transaction: " << beginResult->getErrorMessage()->c_str() << std::endl;
    }

    // Clean up
    std::cout << "\n8. Cleaning up..." << std::endl;
    auto dropFinalResult = executor->execute(oatpp::data::share::StringTemplate(oatpp::String("DROP TABLE TestUsers"), {}), {}, executor->createTypeResolver(), nullptr);
    if(dropFinalResult->isSuccess()) {
      std::cout << "✓ TestUsers table dropped successfully." << std::endl;
    } else {
      std::cout << "✗ Failed to drop table: " << dropFinalResult->getErrorMessage()->c_str() << std::endl;
    }

    std::cout << "\n=== Example completed successfully! ===" << std::endl;
    std::cout << "\nThis example demonstrates:" << std::endl;
    std::cout << "  • SQL Server connection via ODBC" << std::endl;
    std::cout << "  • Table creation and management" << std::endl;
    std::cout << "  • Data insertion, querying, and updates" << std::endl;
    std::cout << "  • Transaction support" << std::endl;
    std::cout << "  • Error handling" << std::endl;
    std::cout << "  • Cross-platform compatibility (Windows/Linux)" << std::endl;

  } catch (const std::exception& e) {
    std::cerr << "\n✗ Error: " << e.what() << std::endl;
    oatpp::Environment::destroy();
    return 1;
  }

  oatpp::Environment::destroy();
  return 0;
}
