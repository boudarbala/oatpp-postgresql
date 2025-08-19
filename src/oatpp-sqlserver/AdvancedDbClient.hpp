/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#ifndef oatpp_sqlserver_AdvancedDbClient_hpp
#define oatpp_sqlserver_AdvancedDbClient_hpp

#include "QueryBuilder.hpp"
#include "oatpp/orm/DbClient.hpp"
#include "oatpp/macro/codegen.hpp"
#include <functional>

namespace oatpp { namespace sqlserver {

/**
 * Advanced SQL Server ORM client with sophisticated features like:
 * - Bulk operations
 * - Stored procedures
 * - Table-valued parameters
 * - Advanced querying with query builder
 * - Performance monitoring
 * - Connection health monitoring
 */
class AdvancedDbClient : public oatpp::orm::DbClient {
public:

  /**
   * Result of a bulk operation.
   */
  struct BulkOperationResult {
    v_int64 affectedRows;
    v_int64 insertedRows;
    v_int64 updatedRows;
    v_int64 deletedRows;
    oatpp::String errorMessage;
    bool success;
    std::chrono::milliseconds executionTime;
  };

  /**
   * Performance metrics for query execution.
   */
  struct QueryMetrics {
    std::chrono::milliseconds executionTime;
    v_int64 rowsAffected;
    v_int64 rowsReturned;
    oatpp::String queryText;
    std::chrono::system_clock::time_point timestamp;
    bool success;
    oatpp::String errorMessage;
  };

  /**
   * Connection health information.
   */
  struct ConnectionHealth {
    bool isConnected;
    std::chrono::milliseconds pingTime;
    v_int64 activeConnections;
    v_int64 totalConnections;
    std::chrono::system_clock::time_point lastHealthCheck;
    oatpp::String serverVersion;
    oatpp::String databaseName;
  };

  /**
   * Pagination information.
   */
  struct PaginationInfo {
    v_int32 pageNumber;
    v_int32 pageSize;
    v_int64 totalRows;
    v_int32 totalPages;
    bool hasNext;
    bool hasPrevious;
  };

  /**
   * Paginated result wrapper.
   */
  template<typename T>
  struct PaginatedResult {
    T data;
    PaginationInfo pagination;
    QueryMetrics metrics;
  };

private:
  std::vector<QueryMetrics> m_queryHistory;
  std::function<void(const QueryMetrics&)> m_metricsCallback;
  bool m_enableMetrics;
  std::chrono::milliseconds m_queryTimeout;

public:

  AdvancedDbClient(const std::shared_ptr<oatpp::orm::Executor>& executor);

  /**
   * Enable or disable performance metrics collection.
   * @param enable - whether to enable metrics
   */
  void enableMetrics(bool enable = true);

  /**
   * Set a callback for query metrics.
   * @param callback - callback function
   */
  void setMetricsCallback(std::function<void(const QueryMetrics&)> callback);

  /**
   * Set query timeout.
   * @param timeout - timeout in milliseconds
   */
  void setQueryTimeout(std::chrono::milliseconds timeout);

  /**
   * Get query execution history.
   * @return vector of query metrics
   */
  std::vector<QueryMetrics> getQueryHistory() const;

  /**
   * Clear query execution history.
   */
  void clearQueryHistory();

  /**
   * Check connection health.
   * @return connection health information
   */
  ConnectionHealth checkHealth();

  /**
   * Execute a query using QueryBuilder with metrics.
   * @param builder - configured query builder
   * @return query result with metrics
   */
  template<typename T>
  std::shared_ptr<T> executeQuery(const QueryBuilder& builder) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
      auto sql = builder.build();
      auto result = this->selectFromQuery<T>(sql);
      
      recordMetrics(sql, startTime, 0, result ? result->size() : 0, true, nullptr);
      return result;
      
    } catch (const std::exception& e) {
      recordMetrics(builder.build(), startTime, 0, 0, false, e.what());
      throw;
    }
  }

  /**
   * Execute paginated query using QueryBuilder.
   * @param builder - configured query builder (without OFFSET/FETCH)
   * @param pageNumber - page number (1-based)
   * @param pageSize - number of rows per page
   * @return paginated result
   */
  template<typename T>
  PaginatedResult<std::shared_ptr<T>> executePaginatedQuery(
      QueryBuilder builder, v_int32 pageNumber, v_int32 pageSize) {
    
    // Get total count first
    auto countBuilder = QueryBuilder()
      .select("COUNT(*)")
      .from("(" + builder.build() + ")", "subquery");
    
    auto countResult = executeQuery<oatpp::Vector<oatpp::Object<dto::Int64Dto>>>(countBuilder);
    v_int64 totalRows = countResult && !countResult->empty() ? 
      countResult->at(0)->value : 0;

    // Calculate pagination info
    PaginationInfo pagination;
    pagination.pageNumber = pageNumber;
    pagination.pageSize = pageSize;
    pagination.totalRows = totalRows;
    pagination.totalPages = static_cast<v_int32>((totalRows + pageSize - 1) / pageSize);
    pagination.hasNext = pageNumber < pagination.totalPages;
    pagination.hasPrevious = pageNumber > 1;

    // Execute paginated query
    v_int32 offset = (pageNumber - 1) * pageSize;
    builder.offset(offset).fetchNext(pageSize);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto data = executeQuery<T>(builder);
    auto endTime = std::chrono::high_resolution_clock::now();

    QueryMetrics metrics;
    metrics.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    metrics.rowsReturned = data ? data->size() : 0;
    metrics.queryText = builder.build();
    metrics.timestamp = std::chrono::system_clock::now();
    metrics.success = true;

    PaginatedResult<std::shared_ptr<T>> result;
    result.data = data;
    result.pagination = pagination;
    result.metrics = metrics;

    return result;
  }

  /**
   * Bulk insert operation.
   * @param tableName - target table name
   * @param data - vector of DTOs to insert
   * @param batchSize - batch size for bulk operation
   * @return bulk operation result
   */
  template<typename T>
  BulkOperationResult bulkInsert(const oatpp::String& tableName, 
                                const oatpp::Vector<oatpp::Object<T>>& data,
                                v_int32 batchSize = 1000) {
    auto startTime = std::chrono::high_resolution_clock::now();
    BulkOperationResult result;
    result.success = true;
    result.insertedRows = 0;

    try {
      // For simplicity, use batched individual inserts
      // Real implementation would use SQL Server's BULK INSERT or Table-Valued Parameters
      
      for (v_int32 i = 0; i < data->size(); i += batchSize) {
        v_int32 endIndex = std::min(i + batchSize, static_cast<v_int32>(data->size()));
        
        InsertBuilder builder;
        builder.into(tableName);
        
        // Extract column names from first object
        if (!data->empty()) {
          auto firstObj = data->at(0);
          // This would need reflection or manual column mapping
          // For now, this is a placeholder for the concept
        }
        
        // Build batch insert
        for (v_int32 j = i; j < endIndex; ++j) {
          // Add values from DTO - this would need proper DTO reflection
          // builder.values(extractValuesFromDto(data->at(j)));
        }
        
        auto sql = builder.build();
        auto insertResult = this->executeQuery(sql);
        if (insertResult && insertResult->isSuccess()) {
          result.insertedRows += (endIndex - i);
        } else {
          result.success = false;
          result.errorMessage = insertResult ? insertResult->getErrorMessage() : "Unknown error";
          break;
        }
      }

      result.affectedRows = result.insertedRows;
      
    } catch (const std::exception& e) {
      result.success = false;
      result.errorMessage = e.what();
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    return result;
  }

  /**
   * Execute stored procedure with parameters.
   * @param procedureName - stored procedure name
   * @param parameters - procedure parameters
   * @return query result
   */
  std::shared_ptr<oatpp::orm::QueryResult> executeStoredProcedure(
      const oatpp::String& procedureName,
      const std::unordered_map<oatpp::String, oatpp::Void>& parameters = {});

  /**
   * Execute stored procedure returning typed result.
   * @param procedureName - stored procedure name
   * @param parameters - procedure parameters
   * @return typed result
   */
  template<typename T>
  std::shared_ptr<T> executeStoredProcedure(
      const oatpp::String& procedureName,
      const std::unordered_map<oatpp::String, oatpp::Void>& parameters = {}) {
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
      // Build EXEC statement
      oatpp::String sql = "EXEC " + procedureName;
      if (!parameters.empty()) {
        sql += " ";
        bool first = true;
        for (const auto& param : parameters) {
          if (!first) sql += ", ";
          sql += "@" + param.first + " = :param_" + param.first;
          first = false;
        }
      }
      
      auto result = this->selectFromQuery<T>(sql, parameters);
      recordMetrics(sql, startTime, 0, result ? result->size() : 0, true, nullptr);
      return result;
      
    } catch (const std::exception& e) {
      recordMetrics("EXEC " + procedureName, startTime, 0, 0, false, e.what());
      throw;
    }
  }

  /**
   * Get table schema information.
   * @param tableName - table name
   * @return table schema details
   */
  oatpp::Object<dto::TableSchema> getTableSchema(const oatpp::String& tableName);

  /**
   * Get database schema information.
   * @return database schema details
   */
  oatpp::Object<dto::DatabaseSchema> getDatabaseSchema();

  /**
   * Analyze query performance and suggest optimizations.
   * @param query - SQL query to analyze
   * @return performance analysis result
   */
  oatpp::Object<dto::QueryAnalysis> analyzeQuery(const oatpp::String& query);

  /**
   * Create a transaction with isolation level control.
   * @param isolationLevel - transaction isolation level
   * @return transaction object
   */
  std::shared_ptr<oatpp::orm::Transaction> beginTransaction(
      const oatpp::String& isolationLevel = "READ COMMITTED");

#include OATPP_CODEGEN_BEGIN(DbClient)

  // Enhanced query methods with SQL Server specific features
  QUERY(getServerInfo, "SELECT @@VERSION as version, @@SERVERNAME as server_name, DB_NAME() as database_name")
  
  QUERY(getTableCount, "SELECT COUNT(*) as table_count FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_TYPE = 'BASE TABLE'")
  
  QUERY(getConnectionInfo, 
        "SELECT "
        "  (SELECT COUNT(*) FROM sys.dm_exec_sessions WHERE is_user_process = 1) as active_connections,"
        "  (SELECT value FROM sys.configurations WHERE name = 'user connections') as max_connections")

#include OATPP_CODEGEN_END(DbClient)

private:

  void recordMetrics(const oatpp::String& query, 
                    std::chrono::high_resolution_clock::time_point startTime,
                    v_int64 rowsAffected, v_int64 rowsReturned,
                    bool success, const char* errorMessage);

  // DTO definitions for internal use
  namespace dto {
    
    #include OATPP_CODEGEN_BEGIN(DTO)
    
    class Int64Dto : public oatpp::DTO {
      DTO_INIT(Int64Dto, DTO)
      DTO_FIELD(Int64, value);
    };

    class ColumnInfo : public oatpp::DTO {
      DTO_INIT(ColumnInfo, DTO)
      DTO_FIELD(String, columnName);
      DTO_FIELD(String, dataType);
      DTO_FIELD(Boolean, isNullable);
      DTO_FIELD(String, defaultValue);
      DTO_FIELD(Int32, maxLength);
      DTO_FIELD(Int32, precision);
      DTO_FIELD(Int32, scale);
    };

    class TableSchema : public oatpp::DTO {
      DTO_INIT(TableSchema, DTO)
      DTO_FIELD(String, tableName);
      DTO_FIELD(String, schemaName);
      DTO_FIELD(Vector<Object<ColumnInfo>>, columns);
      DTO_FIELD(Vector<String>, primaryKeys);
      DTO_FIELD(Vector<String>, indexes);
    };

    class DatabaseSchema : public oatpp::DTO {
      DTO_INIT(DatabaseSchema, DTO)
      DTO_FIELD(String, databaseName);
      DTO_FIELD(String, serverVersion);
      DTO_FIELD(Vector<Object<TableSchema>>, tables);
      DTO_FIELD(Vector<String>, storedProcedures);
      DTO_FIELD(Vector<String>, functions);
    };

    class QueryAnalysis : public oatpp::DTO {
      DTO_INIT(QueryAnalysis, DTO)
      DTO_FIELD(String, queryText);
      DTO_FIELD(Float64, estimatedCost);
      DTO_FIELD(Vector<String>, recommendedIndexes);
      DTO_FIELD(Vector<String>, warnings);
      DTO_FIELD(String, executionPlan);
      DTO_FIELD(Boolean, hasSeqScan);
    };

    #include OATPP_CODEGEN_END(DTO)
  }
};

}}

#endif // oatpp_sqlserver_AdvancedDbClient_hpp