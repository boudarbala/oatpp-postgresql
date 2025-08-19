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

#include "QueryResult.hpp"

namespace oatpp { namespace sqlserver {

QueryResult::QueryResult(HSTMT stmt,
                         const provider::ResourceHandle<orm::Connection>& connection,
                         const std::shared_ptr<mapping::ResultMapper>& resultMapper,
                         const std::shared_ptr<const data::mapping::TypeResolver>& typeResolver)
  : m_stmt(stmt)
  , m_connection(connection)
  , m_resultMapper(resultMapper)
  , m_resultData(stmt, typeResolver)
{
  if(m_stmt == SQL_NULL_HSTMT) {
    m_success = false;
    m_type = TYPE_ERROR;
    return;
  }
  SQLRETURN ret = SQLRowCount(m_stmt, &m_rowCount);
  if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
    if (m_rowCount >= 0) {
      m_success = true;
      m_type = TYPE_COMMAND;
    } else {
      // Check if there are result columns
      SQLSMALLINT numCols;
      ret = SQLNumResultCols(m_stmt, &numCols);
      if (ret == SQL_SUCCESS && numCols > 0) {
        m_success = true;
        m_type = TYPE_TUPLES;
      } else {
        m_success = true;
        m_type = TYPE_COMMAND;
      }
    }
  } else {
    m_success = false;
    m_type = TYPE_ERROR;
  }
}

QueryResult::~QueryResult() {
  if (m_stmt != SQL_NULL_HSTMT) {
    SQLFreeHandle(SQL_HANDLE_STMT, m_stmt);
  }
}

provider::ResourceHandle<orm::Connection> QueryResult::getConnection() const {
  return provider::ResourceHandle<orm::Connection>(m_connection.object, m_connection.invalidator);
}

bool QueryResult::isSuccess() const {
  return m_success;
}

oatpp::String QueryResult::getErrorMessage() const {
  if(!m_success) {
    SQLCHAR sqlState[6], message[256];
    SQLINTEGER nativeError;
    SQLSMALLINT messageLength;
    
    SQLGetDiagRec(SQL_HANDLE_STMT, m_stmt, 1, sqlState, &nativeError, 
                  message, sizeof(message), &messageLength);
    
    return oatpp::String((char*)message, messageLength);
  }
  return nullptr;
}

v_int64 QueryResult::getPosition() const {
  return m_resultData.rowIndex;
}

v_int64 QueryResult::getKnownCount() const {
  switch(m_type) {
    case TYPE_TUPLES: return m_resultData.rowCount;
    case TYPE_COMMAND: return m_rowCount;
  }
  return 0;
}

bool QueryResult::hasMoreToFetch() const {
  return getKnownCount() > 0;
}

oatpp::Void QueryResult::fetch(const oatpp::Type* const resultType, v_int64 count) {
  return m_resultMapper->readRows(&m_resultData, resultType, count);
}

}}
