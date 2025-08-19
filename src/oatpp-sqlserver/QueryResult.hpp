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

#ifndef oatpp_sqlserver_QueryResult_hpp
#define oatpp_sqlserver_QueryResult_hpp

#include "ConnectionProvider.hpp"
#include "mapping/Deserializer.hpp"
#include "mapping/ResultMapper.hpp"
#include "oatpp/orm/QueryResult.hpp"

// Ensure Windows headers included before any ODBC dependent usage in this header's declarations
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

namespace oatpp { namespace sqlserver {


/**
 * Implementation of &id:oatpp::orm::QueryResult;. for SQL Server.
 */
class QueryResult : public orm::QueryResult {
private:
  static constexpr v_int32 TYPE_ERROR = 0;
  static constexpr v_int32 TYPE_COMMAND = 1;
  static constexpr v_int32 TYPE_TUPLES = 2;
private:
  HSTMT m_stmt;
  provider::ResourceHandle<orm::Connection> m_connection;
  std::shared_ptr<mapping::ResultMapper> m_resultMapper;
  mapping::ResultMapper::ResultData m_resultData;
  bool m_success;
  v_int32 m_type;
  SQLLEN m_rowCount;
private:
  mapping::Deserializer m_deserializer;
public:

  QueryResult(HSTMT stmt,
              const provider::ResourceHandle<orm::Connection>& connection,
              const std::shared_ptr<mapping::ResultMapper>& resultMapper,
              const std::shared_ptr<const data::mapping::TypeResolver>& typeResolver);

  ~QueryResult();

  provider::ResourceHandle<orm::Connection> getConnection() const override;

  bool isSuccess() const override;

  oatpp::String getErrorMessage() const override;

  v_int64 getPosition() const override;

  v_int64 getKnownCount() const override;

  bool hasMoreToFetch() const override;

  oatpp::Void fetch(const oatpp::Type* const resultType, v_int64 count) override;

};

}}

#endif //oatpp_sqlserver_QueryResult_hpp
