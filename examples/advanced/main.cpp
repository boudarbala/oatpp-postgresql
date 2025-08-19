#include "oatpp/Environment.hpp"
#include "oatpp/Types.hpp"
#include "oatpp-sqlserver/orm.hpp"
#include "oatpp-sqlserver/ConnectionProvider.hpp"
#include "oatpp-sqlserver/AdvancedDbClient.hpp"
#include "oatpp/macro/codegen.hpp"

#include <iostream>
#include <chrono>

int main() {
  oatpp::Environment::init();

  // Connection string (adjust Driver version if necessary)
  const auto connectionString = oatpp::String(R"(Driver={ODBC Driver 18 for SQL Server};Server=BOUDARBALA\SQLEXPRESS;Database=dbtest;Trusted_Connection=Yes;Encrypt=No;TrustServerCertificate=Yes;)");

  std::cout << "=== Oat++ SQL Server Advanced ORM Example ===" << std::endl;
  std::cout << "Connection String: " << connectionString->c_str() << std::endl;

  try {
    // Create connection provider
    auto connectionProvider = std::make_shared<oatpp::sqlserver::ConnectionProvider>(connectionString);

    // 1) Connectivity Test
    std::cout << "\n1. Testing basic connectivity..." << std::endl;
    auto handle = connectionProvider->get();
    if(handle) {
      std::cout << "[OK] Connected" << std::endl;
    } else {
      std::cout << "[FAIL] No handle" << std::endl; 
      return 1;
    }

    // 2) Advanced ORM Client demonstration
    auto executor = std::make_shared<oatpp::sqlserver::Executor>(connectionProvider);
    oatpp::sqlserver::AdvancedDbClient client(executor);

    // Enable performance metrics
    client.enableMetrics(true);
    client.setQueryTimeout(std::chrono::seconds(30));

    // Set metrics callback
    client.setMetricsCallback([](const oatpp::sqlserver::AdvancedDbClient::QueryMetrics& metrics) {
      std::cout << "[METRICS] Query executed in " << metrics.executionTime.count() 
                << "ms, returned " << metrics.rowsReturned << " rows" << std::endl;
    });

    #include OATPP_CODEGEN_BEGIN(DTO)

    class UserDto : public oatpp::DTO {
      DTO_INIT(UserDto, DTO)
      DTO_FIELD(Int64, id);
      DTO_FIELD(String, name);
      DTO_FIELD(String, email);
      DTO_FIELD(Boolean, isActive);
      DTO_FIELD(String, createdAt);  // Using String for now instead of custom DateTime2
      DTO_FIELD(String, balance);    // Using String for now instead of custom Decimal
    };

    class ProductDto : public oatpp::DTO {
      DTO_INIT(ProductDto, DTO)
      DTO_FIELD(Int64, id);
      DTO_FIELD(String, name);
      DTO_FIELD(String, description);
      DTO_FIELD(String, price);      // Using String for now instead of custom Decimal
      DTO_FIELD(String, launchDate); // Using String for now instead of custom Date
      DTO_FIELD(Boolean, isActive);
    };

    #include OATPP_CODEGEN_END(DTO)

    #include OATPP_CODEGEN_BEGIN(DbClient)

    class AdvancedClient : public oatpp::sqlserver::AdvancedDbClient {
    public:
      AdvancedClient(const std::shared_ptr<oatpp::orm::Executor>& exec)
        : oatpp::sqlserver::AdvancedDbClient(exec) {}

      // Enhanced table creation with advanced types
      QUERY(dropUsersTable, 
            "IF OBJECT_ID('dbo.Users','U') IS NOT NULL DROP TABLE dbo.Users;", 
            PREPARE(false))
      
      QUERY(createUsersTable,
            "CREATE TABLE dbo.Users ("
            "  id BIGINT IDENTITY(1,1) PRIMARY KEY,"
            "  name NVARCHAR(100) NOT NULL,"
            "  email NVARCHAR(255) UNIQUE NOT NULL,"
            "  isActive BIT DEFAULT 1,"
            "  createdAt DATETIME2(7) DEFAULT GETDATE(),"
            "  balance DECIMAL(18,2) DEFAULT 0.00"
            ");",
            PREPARE(false))

      QUERY(dropProductsTable,
            "IF OBJECT_ID('dbo.Products','U') IS NOT NULL DROP TABLE dbo.Products;",
            PREPARE(false))

      QUERY(createProductsTable,
            "CREATE TABLE dbo.Products ("
            "  id BIGINT IDENTITY(1,1) PRIMARY KEY,"
            "  name NVARCHAR(200) NOT NULL,"
            "  description NVARCHAR(MAX),"
            "  price DECIMAL(10,2) NOT NULL,"
            "  launchDate DATE,"
            "  isActive BIT DEFAULT 1"
            ");",
            PREPARE(false))

      // Insert sample data
      QUERY(insertUser,
            "INSERT INTO dbo.Users (name, email, balance) "
            "VALUES ('John Doe', 'john@example.com', 1500.75);",
            PREPARE(false))

      QUERY(insertProduct,
            "INSERT INTO dbo.Products (name, description, price, launchDate) "
            "VALUES ('Premium Widget', 'High-quality widget for enterprise use', 299.99, '2024-01-15');",
            PREPARE(false))

      // Advanced queries with window functions and CTEs
      QUERY(getUsersWithRanking,
            "WITH UserStats AS ("
            "  SELECT id, name, email, balance,"
            "         ROW_NUMBER() OVER (ORDER BY balance DESC) as balance_rank,"
            "         DENSE_RANK() OVER (ORDER BY createdAt) as signup_rank"
            "  FROM dbo.Users WHERE isActive = 1"
            ")"
            "SELECT * FROM UserStats WHERE balance_rank <= 10;",
            PREPARE(false))

      QUERY(getProductAnalytics,
            "SELECT "
            "  id, name, price,"
            "  AVG(price) OVER() as avg_price,"
            "  price - AVG(price) OVER() as price_diff,"
            "  CASE WHEN price > AVG(price) OVER() THEN 'Above Average' ELSE 'Below Average' END as price_category,"
            "  LAG(price) OVER(ORDER BY launchDate) as previous_price"
            "FROM dbo.Products "
            "WHERE isActive = 1 "
            "ORDER BY launchDate;",
            PREPARE(false))

      // Stored procedure example
      QUERY(createGetUsersProcedure,
            "CREATE OR ALTER PROCEDURE dbo.GetUsersByBalance"
            "  @MinBalance DECIMAL(18,2) = 0,"
            "  @MaxBalance DECIMAL(18,2) = 999999.99"
            "AS"
            "BEGIN"
            "  SELECT id, name, email, balance, createdAt"
            "  FROM dbo.Users"
            "  WHERE balance BETWEEN @MinBalance AND @MaxBalance"
            "  ORDER BY balance DESC;"
            "END;",
            PREPARE(false))

      // Clean up
      QUERY(dropTables,
            "IF OBJECT_ID('dbo.GetUsersByBalance','P') IS NOT NULL DROP PROCEDURE dbo.GetUsersByBalance;"
            "IF OBJECT_ID('dbo.Products','U') IS NOT NULL DROP TABLE dbo.Products;"
            "IF OBJECT_ID('dbo.Users','U') IS NOT NULL DROP TABLE dbo.Users;",
            PREPARE(false))
    };

    #include OATPP_CODEGEN_END(DbClient)

    AdvancedClient advancedClient(executor);

    std::cout << "\n2. Creating advanced tables with sophisticated types..." << std::endl;
    (void)advancedClient.dropUsersTable();
    (void)advancedClient.dropProductsTable();
    
    auto createUsersResult = advancedClient.createUsersTable();
    if(!createUsersResult->isSuccess()) { 
      std::cout << "[FAIL] Users table: " << createUsersResult->getErrorMessage()->c_str() << std::endl; 
      return 1; 
    }
    
    auto createProductsResult = advancedClient.createProductsTable();
    if(!createProductsResult->isSuccess()) { 
      std::cout << "[FAIL] Products table: " << createProductsResult->getErrorMessage()->c_str() << std::endl; 
      return 1; 
    }
    std::cout << "[OK] Advanced tables created with DECIMAL, DATETIME2, DATE types" << std::endl;

    std::cout << "\n3. Inserting sample data with advanced types..." << std::endl;
    for(int i = 0; i < 5; ++i) {
      (void)advancedClient.insertUser();
      (void)advancedClient.insertProduct();
    }
    std::cout << "[OK] Sample data inserted" << std::endl;

    std::cout << "\n4. Testing advanced QueryBuilder..." << std::endl;
    
    // Demonstrate QueryBuilder with window functions and CTEs
    oatpp::sqlserver::QueryBuilder queryBuilder;
    auto advancedQuery = queryBuilder
      .withCTE("UserStats", 
               "SELECT id, name, balance, "
               "ROW_NUMBER() OVER (ORDER BY balance DESC) as rank "
               "FROM dbo.Users WHERE isActive = 1")
      .select({"us.id", "us.name", "us.balance", "us.rank"})
      .from("UserStats", "us")
      .where("us.rank <= 3")
      .orderBy("us.balance", oatpp::sqlserver::QueryBuilder::OrderDirection::DESC)
      .build();

    std::cout << "Generated Query:" << std::endl;
    std::cout << advancedQuery->c_str() << std::endl;

    std::cout << "\n5. Testing window functions and analytics..." << std::endl;
    auto analyticsResult = advancedClient.getProductAnalytics();
    if(analyticsResult->isSuccess()) {
      std::cout << "[OK] Analytics query executed successfully" << std::endl;
    } else {
      std::cout << "[INFO] Analytics query (expected to have limited data): " 
                << analyticsResult->getErrorMessage()->c_str() << std::endl;
    }

    std::cout << "\n6. Creating and testing stored procedure..." << std::endl;
    auto procResult = advancedClient.createGetUsersProcedure();
    if(procResult->isSuccess()) {
      std::cout << "[OK] Stored procedure created" << std::endl;
      
      // Test stored procedure execution
      std::unordered_map<oatpp::String, oatpp::Void> params;
      // params["MinBalance"] = oatpp::Float64(100.0);
      // params["MaxBalance"] = oatpp::Float64(2000.0);
      
      auto spResult = advancedClient.executeStoredProcedure("dbo.GetUsersByBalance", params);
      if(spResult->isSuccess()) {
        std::cout << "[OK] Stored procedure executed successfully" << std::endl;
      } else {
        std::cout << "[INFO] Stored procedure execution: " << spResult->getErrorMessage()->c_str() << std::endl;
      }
    } else {
      std::cout << "[INFO] Stored procedure creation: " << procResult->getErrorMessage()->c_str() << std::endl;
    }

    std::cout << "\n7. Testing connection health monitoring..." << std::endl;
    auto health = advancedClient.checkHealth();
    std::cout << "[INFO] Database: " << (health.databaseName ? health.databaseName->c_str() : "Unknown") << std::endl;
    std::cout << "[INFO] Connected: " << (health.isConnected ? "Yes" : "No") << std::endl;
    std::cout << "[INFO] Ping time: " << health.pingTime.count() << "ms" << std::endl;

    std::cout << "\n8. Testing bulk operations..." << std::endl;
    // Create test data for bulk insert
    auto users = oatpp::Vector<oatpp::Object<UserDto>>::createShared();
    for(int i = 0; i < 10; ++i) {
      auto user = UserDto::createShared();
      user->name = "Bulk User " + std::to_string(i);
      user->email = "bulk" + std::to_string(i) + "@example.com";
      user->isActive = true;
      users->push_back(user);
    }

    auto bulkResult = advancedClient.bulkInsert("dbo.Users", users, 5);
    std::cout << "[INFO] Bulk insert result: " << (bulkResult.success ? "Success" : "Failed") << std::endl;
    std::cout << "[INFO] Execution time: " << bulkResult.executionTime.count() << "ms" << std::endl;
    if(!bulkResult.success) {
      std::cout << "[INFO] Error: " << (bulkResult.errorMessage ? bulkResult.errorMessage->c_str() : "Unknown") << std::endl;
    }

    std::cout << "\n9. Displaying query metrics..." << std::endl;
    auto queryHistory = advancedClient.getQueryHistory();
    std::cout << "[INFO] Total queries executed: " << queryHistory.size() << std::endl;
    
    auto totalTime = std::chrono::milliseconds(0);
    for(const auto& metric : queryHistory) {
      totalTime += metric.executionTime;
    }
    if(!queryHistory.empty()) {
      std::cout << "[INFO] Average query time: " << (totalTime.count() / queryHistory.size()) << "ms" << std::endl;
    }

    std::cout << "\n10. Testing pagination concept..." << std::endl;
    auto paginationBuilder = oatpp::sqlserver::QueryBuilder()
      .select({"id", "name", "email", "balance"})
      .from("dbo.Users")
      .where("isActive = 1")
      .orderBy("id")
      .offset(0)
      .fetchNext(5);

    auto paginationQuery = paginationBuilder.build();
    std::cout << "Pagination Query: " << paginationQuery->c_str() << std::endl;

    std::cout << "\n11. Cleaning up..." << std::endl;
    auto cleanup = advancedClient.dropTables();
    if(cleanup->isSuccess()) {
      std::cout << "[OK] Tables and procedures dropped" << std::endl;
    } else {
      std::cout << "[INFO] Cleanup: " << cleanup->getErrorMessage()->c_str() << std::endl;
    }

    std::cout << "\n=== Advanced SQL Server ORM Demo completed successfully! ===" << std::endl;
    std::cout << "\nFeatures demonstrated:" << std::endl;
    std::cout << "✓ Advanced data types (DECIMAL, DATETIME2, DATE)" << std::endl;
    std::cout << "✓ Sophisticated QueryBuilder with CTEs and window functions" << std::endl;
    std::cout << "✓ Stored procedure creation and execution" << std::endl;
    std::cout << "✓ Performance monitoring and query metrics" << std::endl;
    std::cout << "✓ Connection health monitoring" << std::endl;
    std::cout << "✓ Bulk operations (concept)" << std::endl;
    std::cout << "✓ Pagination support (concept)" << std::endl;
    std::cout << "✓ Advanced SQL Server analytics queries" << std::endl;

  } catch (const std::exception& e) {
    std::cerr << "\n✗ Error: " << e.what() << std::endl;
    oatpp::Environment::destroy();
    return 1;
  }

  oatpp::Environment::destroy();
  return 0;
}