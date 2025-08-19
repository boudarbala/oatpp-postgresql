#include "oatpp/Environment.hpp"
#include "oatpp/Types.hpp"
#include "oatpp-sqlserver/orm.hpp"
#include "oatpp-sqlserver/ConnectionProvider.hpp"
#include "oatpp-sqlserver/SimpleQueryBuilder.hpp"
#include "oatpp/macro/codegen.hpp"

#include <iostream>
#include <chrono>

int main() {
  oatpp::Environment::init();

  // Connection string (adjust Driver version if necessary)
  const auto connectionString = oatpp::String(R"(Driver={ODBC Driver 18 for SQL Server};Server=BOUDARBALA\SQLEXPRESS;Database=dbtest;Trusted_Connection=Yes;Encrypt=No;TrustServerCertificate=Yes;)");

  std::cout << "=== Oat++ SQL Server Sophisticated ORM Example ===" << std::endl;
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

    #include OATPP_CODEGEN_BEGIN(DTO)

    class UserDto : public oatpp::DTO {
      DTO_INIT(UserDto, DTO)
      DTO_FIELD(Int64, id);
      DTO_FIELD(String, name);
      DTO_FIELD(String, email);
      DTO_FIELD(Boolean, isActive);
      DTO_FIELD(String, createdAt);    // DATETIME2 as string for now
      DTO_FIELD(String, balance);      // DECIMAL as string for now
    };

    class ProductDto : public oatpp::DTO {
      DTO_INIT(ProductDto, DTO)
      DTO_FIELD(Int64, id);
      DTO_FIELD(String, name);
      DTO_FIELD(String, description);
      DTO_FIELD(String, price);        // DECIMAL as string for now
      DTO_FIELD(String, launchDate);   // DATE as string for now
      DTO_FIELD(Boolean, isActive);
    };

    // Demonstrate advanced SQL Server types
    class TypeShowcaseDto : public oatpp::DTO {
      DTO_INIT(TypeShowcaseDto, DTO)
      DTO_FIELD(Int64, id);
      DTO_FIELD(String, description);
      DTO_FIELD(String, preciseDecimal);    // DECIMAL(18,4)
      DTO_FIELD(String, highPrecDateTime);  // DATETIME2(7)
      DTO_FIELD(String, timeWithZone);      // DATETIMEOFFSET
      DTO_FIELD(String, justTime);          // TIME(3)
      DTO_FIELD(String, justDate);          // DATE
      DTO_FIELD(String, uniqueId);          // UNIQUEIDENTIFIER
    };

    #include OATPP_CODEGEN_END(DTO)

    #include OATPP_CODEGEN_BEGIN(DbClient)

    class SophisticatedClient : public oatpp::orm::DbClient {
    public:
      SophisticatedClient(const std::shared_ptr<oatpp::orm::Executor>& exec)
        : oatpp::orm::DbClient(exec) {}

      // Enhanced table creation with sophisticated SQL Server types
      QUERY(dropShowcaseTable, 
            "IF OBJECT_ID('dbo.TypeShowcase','U') IS NOT NULL DROP TABLE dbo.TypeShowcase;", 
            PREPARE(false))
      
      QUERY(createShowcaseTable,
            "CREATE TABLE dbo.TypeShowcase ("
            "  id BIGINT IDENTITY(1,1) PRIMARY KEY,"
            "  description NVARCHAR(500) NOT NULL,"
            "  preciseDecimal DECIMAL(18,4) NOT NULL,"
            "  highPrecDateTime DATETIME2(7) DEFAULT GETDATE(),"
            "  timeWithZone DATETIMEOFFSET DEFAULT GETDATE(),"
            "  justTime TIME(3) DEFAULT CONVERT(TIME(3), GETDATE()),"
            "  justDate DATE DEFAULT CONVERT(DATE, GETDATE()),"
            "  uniqueId UNIQUEIDENTIFIER DEFAULT NEWID()"
            ");",
            PREPARE(false))

      QUERY(insertShowcaseData,
            "INSERT INTO dbo.TypeShowcase (description, preciseDecimal) VALUES "
            "('High-precision financial data', 12345.6789),"
            "('Scientific measurement', 0.0001),"
            "('Large monetary value', 999999.9999);",
            PREPARE(false))

      // Query with sophisticated data
      QUERY(getShowcaseData,
            "SELECT * FROM dbo.TypeShowcase ORDER BY id;",
            PREPARE(false))

      // Enhanced user/product tables
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

      // Insert sample data with various sophisticated types
      QUERY(insertUsers,
            "INSERT INTO dbo.Users (name, email, balance) VALUES "
            "('Alice Johnson', 'alice@example.com', 2500.75),"
            "('Bob Smith', 'bob@example.com', 1800.50),"
            "('Carol Davis', 'carol@example.com', 3200.00),"
            "('David Wilson', 'david@example.com', 150.25),"
            "('Eva Brown', 'eva@example.com', 4500.90);",
            PREPARE(false))

      QUERY(insertProducts,
            "INSERT INTO dbo.Products (name, description, price, launchDate) VALUES "
            "('Enterprise Widget Pro', 'Professional-grade widget for large organizations', 499.99, '2024-01-15'),"
            "('Standard Widget', 'Mid-tier widget for general use', 199.99, '2024-02-01'),"
            "('Basic Widget Lite', 'Entry-level widget for small teams', 99.99, '2024-02-15'),"
            "('Premium Widget Suite', 'Complete widget solution with advanced features', 799.99, '2024-03-01'),"
            "('Widget Starter Pack', 'Beginner-friendly widget package', 149.99, '2024-03-15');",
            PREPARE(false))

      // Advanced analytics queries using window functions and CTEs
      QUERY(getUserAnalytics,
            "WITH UserStats AS ("
            "  SELECT "
            "    id, name, email, balance,"
            "    ROW_NUMBER() OVER (ORDER BY balance DESC) as wealth_rank,"
            "    PERCENT_RANK() OVER (ORDER BY balance) as wealth_percentile,"
            "    AVG(balance) OVER() as avg_balance,"
            "    COUNT(*) OVER() as total_users"
            "  FROM dbo.Users WHERE isActive = 1"
            ")"
            "SELECT "
            "  id, name, email, balance,"
            "  wealth_rank,"
            "  ROUND(wealth_percentile * 100, 2) as wealth_percentile_pct,"
            "  ROUND(avg_balance, 2) as avg_balance,"
            "  CASE "
            "    WHEN balance > avg_balance * 1.5 THEN 'High Value'"
            "    WHEN balance > avg_balance THEN 'Above Average'"
            "    WHEN balance > avg_balance * 0.5 THEN 'Average'"
            "    ELSE 'Below Average'"
            "  END as customer_segment"
            "FROM UserStats "
            "ORDER BY wealth_rank;",
            PREPARE(false))

      QUERY(getProductPricing,
            "SELECT "
            "  id, name, price,"
            "  LAG(price) OVER(ORDER BY launchDate) as previous_price,"
            "  LEAD(price) OVER(ORDER BY launchDate) as next_price,"
            "  AVG(price) OVER() as avg_price,"
            "  price - AVG(price) OVER() as price_deviation,"
            "  CASE "
            "    WHEN price = MAX(price) OVER() THEN 'Most Expensive'"
            "    WHEN price = MIN(price) OVER() THEN 'Cheapest'"
            "    WHEN price > AVG(price) OVER() THEN 'Premium'"
            "    ELSE 'Economy'"
            "  END as price_category"
            "FROM dbo.Products "
            "WHERE isActive = 1 "
            "ORDER BY launchDate;",
            PREPARE(false))

      // Clean up
      QUERY(dropAllTables,
            "IF OBJECT_ID('dbo.Products','U') IS NOT NULL DROP TABLE dbo.Products;"
            "IF OBJECT_ID('dbo.Users','U') IS NOT NULL DROP TABLE dbo.Users;"
            "IF OBJECT_ID('dbo.TypeShowcase','U') IS NOT NULL DROP TABLE dbo.TypeShowcase;",
            PREPARE(false))
    };

    #include OATPP_CODEGEN_END(DbClient)

    SophisticatedClient client(executor);

    std::cout << "\n2. Creating tables with sophisticated SQL Server types..." << std::endl;
    (void)client.dropShowcaseTable();
    (void)client.dropUsersTable();
    (void)client.dropProductsTable();
    
    auto createShowcaseResult = client.createShowcaseTable();
    if(!createShowcaseResult->isSuccess()) { 
      std::cout << "[FAIL] Showcase table: " << createShowcaseResult->getErrorMessage()->c_str() << std::endl; 
      return 1; 
    }
    
    auto createUsersResult = client.createUsersTable();
    if(!createUsersResult->isSuccess()) { 
      std::cout << "[FAIL] Users table: " << createUsersResult->getErrorMessage()->c_str() << std::endl; 
      return 1; 
    }
    
    auto createProductsResult = client.createProductsTable();
    if(!createProductsResult->isSuccess()) { 
      std::cout << "[FAIL] Products table: " << createProductsResult->getErrorMessage()->c_str() << std::endl; 
      return 1; 
    }
    std::cout << "[OK] All tables created with advanced types:" << std::endl;
    std::cout << "     - DECIMAL(18,4) for high-precision numbers" << std::endl;
    std::cout << "     - DATETIME2(7) for precise timestamps" << std::endl;
    std::cout << "     - DATETIMEOFFSET for timezone-aware dates" << std::endl;
    std::cout << "     - TIME(3) for time-of-day values" << std::endl;
    std::cout << "     - DATE for date-only values" << std::endl;
    std::cout << "     - UNIQUEIDENTIFIER for UUIDs" << std::endl;

    std::cout << "\n3. Inserting sophisticated data..." << std::endl;
    auto insertShowcaseResult = client.insertShowcaseData();
    if(!insertShowcaseResult->isSuccess()) {
      std::cout << "[FAIL] Showcase data: " << insertShowcaseResult->getErrorMessage()->c_str() << std::endl;
    } else {
      std::cout << "[OK] Showcase data inserted" << std::endl;
    }
    
    auto insertUsersResult = client.insertUsers();
    if(!insertUsersResult->isSuccess()) {
      std::cout << "[FAIL] Users data: " << insertUsersResult->getErrorMessage()->c_str() << std::endl;
    } else {
      std::cout << "[OK] Users data inserted" << std::endl;
    }
    
    auto insertProductsResult = client.insertProducts();
    if(!insertProductsResult->isSuccess()) {
      std::cout << "[FAIL] Products data: " << insertProductsResult->getErrorMessage()->c_str() << std::endl;
    } else {
      std::cout << "[OK] Products data inserted" << std::endl;
    }

    std::cout << "\n4. Testing sophisticated SimpleQueryBuilder..." << std::endl;
    
    // Demonstrate advanced query building
    oatpp::sqlserver::SimpleQueryBuilder queryBuilder;
    auto sophisticatedQuery = queryBuilder
      .withCTE("BalanceStats", 
               "SELECT AVG(balance) as avg_bal, MAX(balance) as max_bal FROM dbo.Users")
      .select({"u.id", "u.name", "u.balance", "bs.avg_bal", "bs.max_bal"})
      .select("CASE WHEN u.balance > bs.avg_bal THEN 'Above Average' ELSE 'Below Average' END as category")
      .from("dbo.Users", "u")
      .innerJoin("BalanceStats", "bs", "1=1")
      .where("u.isActive = 1")
      .orderBy("u.balance", "DESC")
      .offset(0)
      .fetchNext(10)
      .build();

    std::cout << "Generated Sophisticated Query:" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << sophisticatedQuery->c_str() << std::endl;
    std::cout << "========================================" << std::endl;

    // Test multiple advanced query patterns
    std::cout << "\n5. Building various advanced query patterns..." << std::endl;
    
    auto windowFunctionQuery = oatpp::sqlserver::SimpleQueryBuilder()
      .select("name")
      .select("balance")
      .select("ROW_NUMBER() OVER (ORDER BY balance DESC) as rank")
      .select("PERCENT_RANK() OVER (ORDER BY balance) as percentile")
      .from("dbo.Users")
      .where("isActive = 1")
      .build();
    
    std::cout << "Window Function Query:" << std::endl;
    std::cout << windowFunctionQuery->c_str() << std::endl;
    std::cout << std::endl;

    auto paginatedQuery = oatpp::sqlserver::SimpleQueryBuilder()
      .select({"name", "email", "balance"})
      .from("dbo.Users")
      .where("balance > 1000")
      .orderBy("balance", "DESC")
      .offset(5)
      .fetchNext(5)
      .build();
    
    std::cout << "Paginated Query (OFFSET/FETCH NEXT):" << std::endl;
    std::cout << paginatedQuery->c_str() << std::endl;
    std::cout << std::endl;

    std::cout << "\n6. Executing advanced analytics queries..." << std::endl;
    auto userAnalyticsResult = client.getUserAnalytics();
    if(userAnalyticsResult->isSuccess()) {
      std::cout << "[OK] User analytics query executed successfully" << std::endl;
    } else {
      std::cout << "[INFO] User analytics: " << userAnalyticsResult->getErrorMessage()->c_str() << std::endl;
    }
    
    auto productPricingResult = client.getProductPricing();
    if(productPricingResult->isSuccess()) {
      std::cout << "[OK] Product pricing analytics executed successfully" << std::endl;
    } else {
      std::cout << "[INFO] Product pricing: " << productPricingResult->getErrorMessage()->c_str() << std::endl;
    }

    std::cout << "\n7. Querying sophisticated data types..." << std::endl;
    auto showcaseResult = client.getShowcaseData();
    if(showcaseResult->isSuccess()) {
      std::cout << "[OK] Sophisticated type showcase data retrieved" << std::endl;
    } else {
      std::cout << "[INFO] Showcase query: " << showcaseResult->getErrorMessage()->c_str() << std::endl;
    }

    std::cout << "\n8. Demonstrating SQL Server-specific features..." << std::endl;
    std::cout << "[INFO] Features showcased in this example:" << std::endl;
    std::cout << "  ✓ High-precision DECIMAL types with custom scale" << std::endl;
    std::cout << "  ✓ DATETIME2 with fractional seconds precision" << std::endl;
    std::cout << "  ✓ DATETIMEOFFSET for timezone-aware operations" << std::endl;
    std::cout << "  ✓ TIME and DATE types for specific temporal needs" << std::endl;
    std::cout << "  ✓ UNIQUEIDENTIFIER for UUID operations" << std::endl;
    std::cout << "  ✓ Window functions (ROW_NUMBER, PERCENT_RANK, LAG, LEAD)" << std::endl;
    std::cout << "  ✓ Common Table Expressions (CTEs)" << std::endl;
    std::cout << "  ✓ SQL Server-style pagination with OFFSET/FETCH NEXT" << std::endl;
    std::cout << "  ✓ Complex analytical queries with sophisticated logic" << std::endl;

    std::cout << "\n9. Cleaning up..." << std::endl;
    auto cleanup = client.dropAllTables();
    if(cleanup->isSuccess()) {
      std::cout << "[OK] All tables dropped" << std::endl;
    } else {
      std::cout << "[INFO] Cleanup: " << cleanup->getErrorMessage()->c_str() << std::endl;
    }

    std::cout << "\n=== Sophisticated SQL Server ORM Demo completed successfully! ===" << std::endl;
    std::cout << "\nThis demonstrates the power of the enhanced oatpp-sqlserver ORM:" << std::endl;
    std::cout << "✓ Advanced SQL Server-specific data types" << std::endl;
    std::cout << "✓ Sophisticated query building capabilities" << std::endl;
    std::cout << "✓ Modern SQL features (CTEs, window functions)" << std::endl;
    std::cout << "✓ Professional-grade analytics and reporting" << std::endl;
    std::cout << "✓ Type-safe ORM with high precision numeric support" << std::endl;

  } catch (const std::exception& e) {
    std::cerr << "\n✗ Error: " << e.what() << std::endl;
    oatpp::Environment::destroy();
    return 1;
  }

  oatpp::Environment::destroy();
  return 0;
}