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

#ifndef oatpp_sqlserver_SimpleQueryBuilder_hpp
#define oatpp_sqlserver_SimpleQueryBuilder_hpp

#include "oatpp/Types.hpp"

namespace oatpp { namespace sqlserver {

/**
 * Simplified but functional SQL Server query builder with essential features:
 * - Basic SELECT, FROM, JOIN, WHERE, GROUP BY, HAVING, ORDER BY
 * - Common Table Expressions (CTEs)
 * - SQL Server-style pagination with OFFSET/FETCH NEXT
 * - Window functions support
 */
class SimpleQueryBuilder {
private:
  oatpp::Vector<oatpp::String> m_selectColumns;
  oatpp::String m_fromTable;
  oatpp::String m_fromAlias;
  oatpp::Vector<oatpp::String> m_joins;
  oatpp::Vector<oatpp::String> m_whereConditions;
  oatpp::Vector<oatpp::String> m_groupByColumns;
  oatpp::Vector<oatpp::String> m_havingConditions;
  oatpp::Vector<oatpp::String> m_orderBy;
  oatpp::Vector<oatpp::String> m_ctes;
  v_int32 m_limit;
  v_int32 m_offset;
  bool m_distinct;

public:
  SimpleQueryBuilder();

  /**
   * Add a Common Table Expression (CTE).
   * @param name - CTE name
   * @param query - CTE query
   * @return this builder for chaining
   */
  SimpleQueryBuilder& withCTE(const oatpp::String& name, const oatpp::String& query);

  /**
   * Set SELECT DISTINCT.
   * @param distinct - whether to use DISTINCT
   * @return this builder for chaining
   */
  SimpleQueryBuilder& distinct(bool distinct = true);

  /**
   * Add a column to SELECT clause.
   * @param column - column expression
   * @return this builder for chaining
   */
  SimpleQueryBuilder& select(const oatpp::String& column);

  /**
   * Add multiple columns to SELECT clause.
   * @param columns - vector of column expressions
   * @return this builder for chaining
   */
  SimpleQueryBuilder& select(const oatpp::Vector<oatpp::String>& columns);

  /**
   * Set FROM table.
   * @param table - table name
   * @param alias - table alias
   * @return this builder for chaining
   */
  SimpleQueryBuilder& from(const oatpp::String& table, const oatpp::String& alias = nullptr);

  /**
   * Add an INNER JOIN clause.
   * @param table - table to join
   * @param alias - table alias
   * @param condition - join condition
   * @return this builder for chaining
   */
  SimpleQueryBuilder& innerJoin(const oatpp::String& table, const oatpp::String& alias, 
                               const oatpp::String& condition);

  /**
   * Add a LEFT JOIN clause.
   * @param table - table to join
   * @param alias - table alias
   * @param condition - join condition
   * @return this builder for chaining
   */
  SimpleQueryBuilder& leftJoin(const oatpp::String& table, const oatpp::String& alias, 
                              const oatpp::String& condition);

  /**
   * Add a WHERE condition.
   * @param condition - WHERE condition
   * @return this builder for chaining
   */
  SimpleQueryBuilder& where(const oatpp::String& condition);

  /**
   * Add an AND WHERE condition.
   * @param condition - WHERE condition
   * @return this builder for chaining
   */
  SimpleQueryBuilder& andWhere(const oatpp::String& condition);

  /**
   * Add a GROUP BY column.
   * @param column - column to group by
   * @return this builder for chaining
   */
  SimpleQueryBuilder& groupBy(const oatpp::String& column);

  /**
   * Add a HAVING condition.
   * @param condition - HAVING condition
   * @return this builder for chaining
   */
  SimpleQueryBuilder& having(const oatpp::String& condition);

  /**
   * Add an ORDER BY clause.
   * @param column - column to order by
   * @param direction - sort direction ("ASC" or "DESC")
   * @return this builder for chaining
   */
  SimpleQueryBuilder& orderBy(const oatpp::String& column, const oatpp::String& direction = "ASC");

  /**
   * Set OFFSET for pagination.
   * @param offset - number of rows to skip
   * @return this builder for chaining
   */
  SimpleQueryBuilder& offset(v_int32 offset);

  /**
   * Set FETCH NEXT (SQL Server's LIMIT equivalent).
   * @param limit - number of rows to fetch
   * @return this builder for chaining
   */
  SimpleQueryBuilder& fetchNext(v_int32 limit);

  /**
   * Build the final SQL query.
   * @return complete SQL query string
   */
  oatpp::String build() const;

  /**
   * Reset the builder to initial state.
   * @return this builder for chaining
   */
  SimpleQueryBuilder& reset();
};

}}

#endif // oatpp_sqlserver_SimpleQueryBuilder_hpp