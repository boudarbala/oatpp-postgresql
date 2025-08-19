# Advanced SQL Server ORM Example

This example demonstrates the sophisticated features of the enhanced oatpp-sqlserver ORM, showcasing how to build professional-grade database applications with modern SQL Server features.

## Features Demonstrated

### 🔧 Advanced SQL Server Data Types
- **DECIMAL(18,4)** - High-precision numeric types for financial calculations
- **DATETIME2(7)** - Precise timestamps with fractional seconds (up to 7 digits)
- **DATETIMEOFFSET** - Timezone-aware date/time values
- **TIME(3)** - Time-of-day values with millisecond precision
- **DATE** - Date-only values without time component
- **UNIQUEIDENTIFIER** - SQL Server UUIDs for global unique identification

### 🏗️ Sophisticated Query Building
- **SimpleQueryBuilder** - Fluent API for building complex SQL queries
- **Common Table Expressions (CTEs)** - Recursive and non-recursive CTEs
- **Window Functions** - ROW_NUMBER(), PERCENT_RANK(), LAG(), LEAD(), AVG() OVER()
- **SQL Server Pagination** - Native OFFSET/FETCH NEXT syntax
- **Complex Joins** - INNER, LEFT, RIGHT joins with sophisticated conditions

### 📊 Advanced Analytics Features
- **Customer Segmentation** - Automatic categorization based on data patterns
- **Ranking and Percentiles** - Statistical analysis with window functions
- **Comparative Analysis** - Price comparisons with LAG/LEAD functions
- **Aggregation Patterns** - Advanced GROUP BY with HAVING clauses

### 🎯 Modern SQL Patterns
- **Type-Safe ORM** - Full oatpp integration with custom SQL Server types
- **Fluent Query API** - Method chaining for readable query construction
- **Professional Schema Design** - Best practices for SQL Server table design
- **Error Handling** - Comprehensive error reporting and diagnostics

## Building and Running

```bash
cd /path/to/oatpp-postgresql/build
make -j$(nproc)
./examples/advanced/advanced-sqlserver
```

## Code Highlights

### Advanced Type Usage
```cpp
// High-precision financial calculations
DECIMAL(18,4) preciseDecimal

// Precise timestamps with fractional seconds
DATETIME2(7) highPrecDateTime

// Timezone-aware operations
DATETIMEOFFSET timeWithZone

// Time-only values
TIME(3) justTime

// Date-only values
DATE justDate

// Global unique identifiers
UNIQUEIDENTIFIER uniqueId
```

### Sophisticated Query Building
```cpp
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
```

### Advanced Analytics Queries
```sql
-- Customer segmentation with window functions
WITH UserStats AS (
  SELECT 
    id, name, email, balance,
    ROW_NUMBER() OVER (ORDER BY balance DESC) as wealth_rank,
    PERCENT_RANK() OVER (ORDER BY balance) as wealth_percentile,
    AVG(balance) OVER() as avg_balance,
    COUNT(*) OVER() as total_users
  FROM dbo.Users WHERE isActive = 1
)
SELECT 
  id, name, email, balance,
  wealth_rank,
  ROUND(wealth_percentile * 100, 2) as wealth_percentile_pct,
  ROUND(avg_balance, 2) as avg_balance,
  CASE 
    WHEN balance > avg_balance * 1.5 THEN 'High Value'
    WHEN balance > avg_balance THEN 'Above Average'
    WHEN balance > avg_balance * 0.5 THEN 'Average'
    ELSE 'Below Average'
  END as customer_segment
FROM UserStats 
ORDER BY wealth_rank;
```

## Schema Design

The example creates sophisticated table schemas that showcase SQL Server's advanced features:

```sql
CREATE TABLE dbo.TypeShowcase (
  id BIGINT IDENTITY(1,1) PRIMARY KEY,
  description NVARCHAR(500) NOT NULL,
  preciseDecimal DECIMAL(18,4) NOT NULL,
  highPrecDateTime DATETIME2(7) DEFAULT GETDATE(),
  timeWithZone DATETIMEOFFSET DEFAULT GETDATE(),
  justTime TIME(3) DEFAULT CONVERT(TIME(3), GETDATE()),
  justDate DATE DEFAULT CONVERT(DATE, GETDATE()),
  uniqueId UNIQUEIDENTIFIER DEFAULT NEWID()
);
```

## Professional Applications

This enhanced ORM is suitable for:

- **Financial Systems** - High-precision decimal calculations
- **Global Applications** - Timezone-aware date/time handling  
- **Analytics Platforms** - Advanced window functions and CTEs
- **Enterprise Software** - Professional schema design patterns
- **Reporting Systems** - Sophisticated data analysis capabilities

## Next Steps

To extend this example further, consider:

1. **Stored Procedures** - Add complex business logic procedures
2. **Bulk Operations** - Implement high-performance data loading
3. **Performance Monitoring** - Add query execution metrics
4. **Connection Pooling** - Optimize database connections
5. **Schema Migrations** - Implement version-controlled schema changes

This example demonstrates how oatpp-sqlserver has evolved into a sophisticated, production-ready ORM capable of handling enterprise-grade SQL Server applications with modern development practices.