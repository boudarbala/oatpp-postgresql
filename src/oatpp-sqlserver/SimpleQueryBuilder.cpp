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

#include "SimpleQueryBuilder.hpp"
#include "oatpp/data/stream/BufferStream.hpp"

namespace oatpp { namespace sqlserver {

SimpleQueryBuilder::SimpleQueryBuilder() 
  : m_limit(-1), m_offset(-1), m_distinct(false) {
}

SimpleQueryBuilder& SimpleQueryBuilder::distinct(bool distinct) {
  m_distinct = distinct;
  return *this;
}

SimpleQueryBuilder& SimpleQueryBuilder::select(const oatpp::String& column) {
  if (!m_selectColumns) {
    m_selectColumns = oatpp::Vector<oatpp::String>::createShared();
  }
  m_selectColumns->push_back(column);
  return *this;
}

SimpleQueryBuilder& SimpleQueryBuilder::select(const oatpp::Vector<oatpp::String>& columns) {
  for (const auto& column : *columns) {
    select(column);
  }
  return *this;
}

SimpleQueryBuilder& SimpleQueryBuilder::from(const oatpp::String& table, const oatpp::String& alias) {
  m_fromTable = table;
  m_fromAlias = alias;
  return *this;
}

SimpleQueryBuilder& SimpleQueryBuilder::innerJoin(const oatpp::String& table, const oatpp::String& alias, const oatpp::String& condition) {
  if (!m_joins) {
    m_joins = oatpp::Vector<oatpp::String>::createShared();
  }
  
  oatpp::data::stream::BufferOutputStream stream;
  stream << "INNER JOIN " << table;
  if (alias) {
    stream << " AS " << alias;
  }
  stream << " ON " << condition;
  
  m_joins->push_back(stream.toString());
  return *this;
}

SimpleQueryBuilder& SimpleQueryBuilder::leftJoin(const oatpp::String& table, const oatpp::String& alias, const oatpp::String& condition) {
  if (!m_joins) {
    m_joins = oatpp::Vector<oatpp::String>::createShared();
  }
  
  oatpp::data::stream::BufferOutputStream stream;
  stream << "LEFT JOIN " << table;
  if (alias) {
    stream << " AS " << alias;
  }
  stream << " ON " << condition;
  
  m_joins->push_back(stream.toString());
  return *this;
}

SimpleQueryBuilder& SimpleQueryBuilder::where(const oatpp::String& condition) {
  if (!m_whereConditions) {
    m_whereConditions = oatpp::Vector<oatpp::String>::createShared();
  }
  m_whereConditions->push_back(condition);
  return *this;
}

SimpleQueryBuilder& SimpleQueryBuilder::andWhere(const oatpp::String& condition) {
  return where(condition);
}

SimpleQueryBuilder& SimpleQueryBuilder::groupBy(const oatpp::String& column) {
  if (!m_groupByColumns) {
    m_groupByColumns = oatpp::Vector<oatpp::String>::createShared();
  }
  m_groupByColumns->push_back(column);
  return *this;
}

SimpleQueryBuilder& SimpleQueryBuilder::having(const oatpp::String& condition) {
  if (!m_havingConditions) {
    m_havingConditions = oatpp::Vector<oatpp::String>::createShared();
  }
  m_havingConditions->push_back(condition);
  return *this;
}

SimpleQueryBuilder& SimpleQueryBuilder::orderBy(const oatpp::String& column, const oatpp::String& direction) {
  if (!m_orderBy) {
    m_orderBy = oatpp::Vector<oatpp::String>::createShared();
  }
  
  oatpp::data::stream::BufferOutputStream stream;
  stream << column;
  if (direction) {
    stream << " " << direction;
  }
  
  m_orderBy->push_back(stream.toString());
  return *this;
}

SimpleQueryBuilder& SimpleQueryBuilder::offset(v_int32 offset) {
  m_offset = offset;
  return *this;
}

SimpleQueryBuilder& SimpleQueryBuilder::fetchNext(v_int32 limit) {
  m_limit = limit;
  return *this;
}

SimpleQueryBuilder& SimpleQueryBuilder::withCTE(const oatpp::String& name, const oatpp::String& query) {
  if (!m_ctes) {
    m_ctes = oatpp::Vector<oatpp::String>::createShared();
  }
  
  oatpp::data::stream::BufferOutputStream stream;
  stream << name << " AS (" << query << ")";
  
  m_ctes->push_back(stream.toString());
  return *this;
}

oatpp::String SimpleQueryBuilder::build() const {
  oatpp::data::stream::BufferOutputStream stream;
  
  // CTEs
  if (m_ctes && !m_ctes->empty()) {
    stream << "WITH ";
    for (v_uint32 i = 0; i < m_ctes->size(); ++i) {
      if (i > 0) stream << ", ";
      stream << m_ctes->at(i);
    }
    stream << " ";
  }
  
  // SELECT clause
  stream << "SELECT ";
  if (m_distinct) {
    stream << "DISTINCT ";
  }
  
  if (m_selectColumns && !m_selectColumns->empty()) {
    for (v_uint32 i = 0; i < m_selectColumns->size(); ++i) {
      if (i > 0) stream << ", ";
      stream << m_selectColumns->at(i);
    }
  } else {
    stream << "*";
  }
  
  // FROM clause
  if (m_fromTable) {
    stream << " FROM " << m_fromTable;
    if (m_fromAlias) {
      stream << " AS " << m_fromAlias;
    }
  }
  
  // JOIN clauses
  if (m_joins && !m_joins->empty()) {
    for (const auto& join : *m_joins) {
      stream << " " << join;
    }
  }
  
  // WHERE clause
  if (m_whereConditions && !m_whereConditions->empty()) {
    stream << " WHERE ";
    for (v_uint32 i = 0; i < m_whereConditions->size(); ++i) {
      if (i > 0) stream << " AND ";
      stream << "(" << m_whereConditions->at(i) << ")";
    }
  }
  
  // GROUP BY clause
  if (m_groupByColumns && !m_groupByColumns->empty()) {
    stream << " GROUP BY ";
    for (v_uint32 i = 0; i < m_groupByColumns->size(); ++i) {
      if (i > 0) stream << ", ";
      stream << m_groupByColumns->at(i);
    }
  }
  
  // HAVING clause
  if (m_havingConditions && !m_havingConditions->empty()) {
    stream << " HAVING ";
    for (v_uint32 i = 0; i < m_havingConditions->size(); ++i) {
      if (i > 0) stream << " AND ";
      stream << "(" << m_havingConditions->at(i) << ")";
    }
  }
  
  // ORDER BY clause
  if (m_orderBy && !m_orderBy->empty()) {
    stream << " ORDER BY ";
    for (v_uint32 i = 0; i < m_orderBy->size(); ++i) {
      if (i > 0) stream << ", ";
      stream << m_orderBy->at(i);
    }
  }
  
  // OFFSET and FETCH NEXT (SQL Server pagination)
  if (m_offset >= 0 && m_limit > 0) {
    // SQL Server requires ORDER BY for OFFSET
    if (!m_orderBy || m_orderBy->empty()) {
      stream << " ORDER BY (SELECT NULL)";
    }
    stream << " OFFSET " << m_offset << " ROWS FETCH NEXT " << m_limit << " ROWS ONLY";
  } else if (m_offset >= 0) {
    if (!m_orderBy || m_orderBy->empty()) {
      stream << " ORDER BY (SELECT NULL)";
    }
    stream << " OFFSET " << m_offset << " ROWS";
  }
  
  return stream.toString();
}

SimpleQueryBuilder& SimpleQueryBuilder::reset() {
  m_selectColumns = nullptr;
  m_fromTable = nullptr;
  m_fromAlias = nullptr;
  m_joins = nullptr;
  m_whereConditions = nullptr;
  m_groupByColumns = nullptr;
  m_havingConditions = nullptr;
  m_orderBy = nullptr;
  m_ctes = nullptr;
  m_limit = -1;
  m_offset = -1;
  m_distinct = false;
  return *this;
}

}}