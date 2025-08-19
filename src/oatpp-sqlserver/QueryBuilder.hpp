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

#ifndef oatpp_sqlserver_QueryBuilder_hpp
#define oatpp_sqlserver_QueryBuilder_hpp

#include "oatpp/Types.hpp"
#include <vector>
#include <unordered_map>
#include <sstream>
#include <memory>

namespace oatpp { namespace sqlserver {

/**
 * Advanced SQL Server query builder with support for complex queries,
 * CTEs, window functions, and SQL Server specific features.
 */
class QueryBuilder {
public:
  
  enum class JoinType {
    INNER,
    LEFT,
    RIGHT,
    FULL,
    CROSS
  };

  enum class OrderDirection {
    ASC,
    DESC
  };

  enum class WindowFrameType {
    ROWS,
    RANGE
  };

  enum class WindowFrameBound {
    UNBOUNDED_PRECEDING,
    CURRENT_ROW,
    UNBOUNDED_FOLLOWING,
    PRECEDING,
    FOLLOWING
  };

  struct JoinClause {
    JoinType type;
    oatpp::String table;
    oatpp::String alias;
    oatpp::String condition;
  };

  struct OrderByClause {
    oatpp::String expression;
    OrderDirection direction;
  };

  struct WindowFunction {
    oatpp::String functionName;
    oatpp::Vector<oatpp::String> arguments;
    oatpp::Vector<oatpp::String> partitionBy;
    oatpp::Vector<OrderByClause> orderBy;
    WindowFrameType frameType;
    WindowFrameBound frameStart;
    WindowFrameBound frameEnd;
    oatpp::String alias;
  };

  struct CTE {
    oatpp::String name;
    oatpp::String query;
    bool recursive;
  };

private:
  oatpp::Vector<oatpp::String> m_selectColumns;
  oatpp::String m_fromTable;
  oatpp::String m_fromAlias;
  oatpp::Vector<JoinClause> m_joins;
  oatpp::Vector<oatpp::String> m_whereConditions;
  oatpp::Vector<oatpp::String> m_groupByColumns;
  oatpp::Vector<oatpp::String> m_havingConditions;
  oatpp::Vector<OrderByClause> m_orderBy;
  oatpp::Vector<WindowFunction> m_windowFunctions;
  oatpp::Vector<CTE> m_ctes;
  v_int32 m_limit;
  v_int32 m_offset;
  bool m_distinct;
  bool m_forUpdate;
  oatpp::String m_tableHints;

public:
  QueryBuilder();

  /**
   * Add a Common Table Expression (CTE).
   * @param name - CTE name
   * @param query - CTE query
   * @param recursive - whether the CTE is recursive
   * @return this builder for chaining
   */
  QueryBuilder& withCTE(const oatpp::String& name, const oatpp::String& query, bool recursive = false);

  /**
   * Set SELECT DISTINCT.
   * @param distinct - whether to use DISTINCT
   * @return this builder for chaining
   */
  QueryBuilder& distinct(bool distinct = true);

  /**
   * Add a column to SELECT clause.
   * @param column - column expression
   * @return this builder for chaining
   */
  QueryBuilder& select(const oatpp::String& column);

  /**
   * Add multiple columns to SELECT clause.
   * @param columns - vector of column expressions
   * @return this builder for chaining
   */
  QueryBuilder& select(const oatpp::Vector<oatpp::String>& columns);

  /**
   * Add a window function to SELECT clause.
   * @param functionName - window function name (ROW_NUMBER, RANK, etc.)
   * @param alias - alias for the function result
   * @return this builder for chaining
   */
  QueryBuilder& selectWindowFunction(const oatpp::String& functionName, const oatpp::String& alias);

  /**
   * Set FROM table.
   * @param table - table name
   * @param alias - table alias
   * @return this builder for chaining
   */
  QueryBuilder& from(const oatpp::String& table, const oatpp::String& alias = nullptr);

  /**
   * Add table hints (SQL Server specific).
   * @param hints - table hints (e.g., "WITH (NOLOCK)")
   * @return this builder for chaining
   */
  QueryBuilder& withTableHints(const oatpp::String& hints);

  /**
   * Add a JOIN clause.
   * @param type - join type
   * @param table - table to join
   * @param alias - table alias
   * @param condition - join condition
   * @return this builder for chaining
   */
  QueryBuilder& join(JoinType type, const oatpp::String& table, 
                    const oatpp::String& alias, const oatpp::String& condition);

  /**
   * Add an INNER JOIN clause.
   * @param table - table to join
   * @param alias - table alias
   * @param condition - join condition
   * @return this builder for chaining
   */
  QueryBuilder& innerJoin(const oatpp::String& table, const oatpp::String& alias, 
                         const oatpp::String& condition);

  /**
   * Add a LEFT JOIN clause.
   * @param table - table to join
   * @param alias - table alias
   * @param condition - join condition
   * @return this builder for chaining
   */
  QueryBuilder& leftJoin(const oatpp::String& table, const oatpp::String& alias, 
                        const oatpp::String& condition);

  /**
   * Add a WHERE condition.
   * @param condition - WHERE condition
   * @return this builder for chaining
   */
  QueryBuilder& where(const oatpp::String& condition);

  /**
   * Add an AND WHERE condition.
   * @param condition - WHERE condition
   * @return this builder for chaining
   */
  QueryBuilder& andWhere(const oatpp::String& condition);

  /**
   * Add an OR WHERE condition.
   * @param condition - WHERE condition
   * @return this builder for chaining
   */
  QueryBuilder& orWhere(const oatpp::String& condition);

  /**
   * Add a GROUP BY column.
   * @param column - column to group by
   * @return this builder for chaining
   */
  QueryBuilder& groupBy(const oatpp::String& column);

  /**
   * Add multiple GROUP BY columns.
   * @param columns - columns to group by
   * @return this builder for chaining
   */
  QueryBuilder& groupBy(const oatpp::Vector<oatpp::String>& columns);

  /**
   * Add a HAVING condition.
   * @param condition - HAVING condition
   * @return this builder for chaining
   */
  QueryBuilder& having(const oatpp::String& condition);

  /**
   * Add an ORDER BY clause.
   * @param column - column to order by
   * @param direction - sort direction
   * @return this builder for chaining
   */
  QueryBuilder& orderBy(const oatpp::String& column, OrderDirection direction = OrderDirection::ASC);

  /**
   * Set OFFSET for pagination.
   * @param offset - number of rows to skip
   * @return this builder for chaining
   */
  QueryBuilder& offset(v_int32 offset);

  /**
   * Set FETCH NEXT (SQL Server's LIMIT equivalent).
   * @param limit - number of rows to fetch
   * @return this builder for chaining
   */
  QueryBuilder& fetchNext(v_int32 limit);

  /**
   * Add FOR UPDATE clause.
   * @param forUpdate - whether to add FOR UPDATE
   * @return this builder for chaining
   */
  QueryBuilder& forUpdate(bool forUpdate = true);

  /**
   * Configure a window function with partition and order.
   * @param functionName - window function name
   * @param partitionBy - columns to partition by
   * @param orderBy - columns to order by
   * @param alias - alias for the function result
   * @return this builder for chaining
   */
  QueryBuilder& windowFunction(const oatpp::String& functionName,
                              const oatpp::Vector<oatpp::String>& partitionBy,
                              const oatpp::Vector<OrderByClause>& orderBy,
                              const oatpp::String& alias);

  /**
   * Build the final SQL query.
   * @return complete SQL query string
   */
  oatpp::String build() const;

  /**
   * Reset the builder to initial state.
   * @return this builder for chaining
   */
  QueryBuilder& reset();

private:
  oatpp::String joinTypeToString(JoinType type) const;
  oatpp::String orderDirectionToString(OrderDirection direction) const;
  oatpp::String buildSelectClause() const;
  oatpp::String buildFromClause() const;
  oatpp::String buildJoinClause() const;
  oatpp::String buildWhereClause() const;
  oatpp::String buildGroupByClause() const;
  oatpp::String buildHavingClause() const;
  oatpp::String buildOrderByClause() const;
  oatpp::String buildLimitClause() const;
  oatpp::String buildCTEClause() const;
  oatpp::String buildWindowFunctions() const;
};

/**
 * Builder for INSERT statements with advanced SQL Server features.
 */
class InsertBuilder {
private:
  oatpp::String m_table;
  oatpp::Vector<oatpp::String> m_columns;
  oatpp::Vector<oatpp::Vector<oatpp::String>> m_values;
  oatpp::String m_selectQuery;
  oatpp::Vector<oatpp::String> m_outputColumns;
  bool m_ignoreConflicts;

public:
  InsertBuilder();

  /**
   * Set the target table.
   * @param table - table name
   * @return this builder for chaining
   */
  InsertBuilder& into(const oatpp::String& table);

  /**
   * Set the columns to insert into.
   * @param columns - column names
   * @return this builder for chaining
   */
  InsertBuilder& columns(const oatpp::Vector<oatpp::String>& columns);

  /**
   * Add a row of values.
   * @param values - values for the row
   * @return this builder for chaining
   */
  InsertBuilder& values(const oatpp::Vector<oatpp::String>& values);

  /**
   * Set a SELECT query as the source of values.
   * @param selectQuery - SELECT query
   * @return this builder for chaining
   */
  InsertBuilder& fromSelect(const oatpp::String& selectQuery);

  /**
   * Add OUTPUT clause (SQL Server specific).
   * @param columns - columns to output
   * @return this builder for chaining
   */
  InsertBuilder& output(const oatpp::Vector<oatpp::String>& columns);

  /**
   * Set whether to ignore conflicts (use ON DUPLICATE KEY IGNORE equivalent).
   * @param ignore - whether to ignore conflicts
   * @return this builder for chaining
   */
  InsertBuilder& ignoreConflicts(bool ignore = true);

  /**
   * Build the final INSERT statement.
   * @return complete INSERT SQL
   */
  oatpp::String build() const;

  /**
   * Reset the builder to initial state.
   * @return this builder for chaining
   */
  InsertBuilder& reset();
};

/**
 * Builder for UPDATE statements with advanced SQL Server features.
 */
class UpdateBuilder {
private:
  oatpp::String m_table;
  oatpp::String m_alias;
  oatpp::Vector<std::pair<oatpp::String, oatpp::String>> m_setClause;
  oatpp::Vector<JoinClause> m_joins;
  oatpp::Vector<oatpp::String> m_whereConditions;
  oatpp::Vector<oatpp::String> m_outputColumns;

public:
  UpdateBuilder();

  /**
   * Set the target table.
   * @param table - table name
   * @param alias - table alias
   * @return this builder for chaining
   */
  UpdateBuilder& table(const oatpp::String& table, const oatpp::String& alias = nullptr);

  /**
   * Add a SET clause.
   * @param column - column to update
   * @param value - new value expression
   * @return this builder for chaining
   */
  UpdateBuilder& set(const oatpp::String& column, const oatpp::String& value);

  /**
   * Add a JOIN clause for update with join.
   * @param type - join type
   * @param table - table to join
   * @param alias - table alias
   * @param condition - join condition
   * @return this builder for chaining
   */
  UpdateBuilder& join(QueryBuilder::JoinType type, const oatpp::String& table, 
                     const oatpp::String& alias, const oatpp::String& condition);

  /**
   * Add a WHERE condition.
   * @param condition - WHERE condition
   * @return this builder for chaining
   */
  UpdateBuilder& where(const oatpp::String& condition);

  /**
   * Add OUTPUT clause (SQL Server specific).
   * @param columns - columns to output
   * @return this builder for chaining
   */
  UpdateBuilder& output(const oatpp::Vector<oatpp::String>& columns);

  /**
   * Build the final UPDATE statement.
   * @return complete UPDATE SQL
   */
  oatpp::String build() const;

  /**
   * Reset the builder to initial state.
   * @return this builder for chaining
   */
  UpdateBuilder& reset();

private:
  oatpp::String joinTypeToString(QueryBuilder::JoinType type) const;
};

/**
 * Builder for advanced SQL Server features like MERGE statements.
 */
class MergeBuilder {
private:
  oatpp::String m_targetTable;
  oatpp::String m_targetAlias;
  oatpp::String m_sourceTable;
  oatpp::String m_sourceAlias;
  oatpp::String m_joinCondition;
  oatpp::Vector<std::pair<oatpp::String, oatpp::String>> m_updateSet;
  oatpp::Vector<oatpp::String> m_insertColumns;
  oatpp::Vector<oatpp::String> m_insertValues;
  oatpp::String m_updateCondition;
  oatpp::String m_insertCondition;
  oatpp::String m_deleteCondition;
  oatpp::Vector<oatpp::String> m_outputColumns;

public:
  MergeBuilder();

  /**
   * Set the target table.
   * @param table - target table name
   * @param alias - target table alias
   * @return this builder for chaining
   */
  MergeBuilder& target(const oatpp::String& table, const oatpp::String& alias = nullptr);

  /**
   * Set the source table.
   * @param table - source table name
   * @param alias - source table alias
   * @return this builder for chaining
   */
  MergeBuilder& source(const oatpp::String& table, const oatpp::String& alias = nullptr);

  /**
   * Set the join condition between target and source.
   * @param condition - join condition
   * @return this builder for chaining
   */
  MergeBuilder& on(const oatpp::String& condition);

  /**
   * Add WHEN MATCHED THEN UPDATE clause.
   * @param setClause - columns to update
   * @param condition - additional condition
   * @return this builder for chaining
   */
  MergeBuilder& whenMatchedUpdate(const oatpp::Vector<std::pair<oatpp::String, oatpp::String>>& setClause,
                                 const oatpp::String& condition = nullptr);

  /**
   * Add WHEN NOT MATCHED THEN INSERT clause.
   * @param columns - columns to insert
   * @param values - values to insert
   * @param condition - additional condition
   * @return this builder for chaining
   */
  MergeBuilder& whenNotMatchedInsert(const oatpp::Vector<oatpp::String>& columns,
                                    const oatpp::Vector<oatpp::String>& values,
                                    const oatpp::String& condition = nullptr);

  /**
   * Add WHEN NOT MATCHED BY SOURCE THEN DELETE clause.
   * @param condition - additional condition
   * @return this builder for chaining
   */
  MergeBuilder& whenNotMatchedBySourceDelete(const oatpp::String& condition = nullptr);

  /**
   * Add OUTPUT clause.
   * @param columns - columns to output
   * @return this builder for chaining
   */
  MergeBuilder& output(const oatpp::Vector<oatpp::String>& columns);

  /**
   * Build the final MERGE statement.
   * @return complete MERGE SQL
   */
  oatpp::String build() const;

  /**
   * Reset the builder to initial state.
   * @return this builder for chaining
   */
  MergeBuilder& reset();
};

}}

#endif // oatpp_sqlserver_QueryBuilder_hpp