/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\\  )( (_   _)(_   _)
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

#include "Executor.hpp"
#include "QueryResult.hpp"
#include "oatpp/orm/Transaction.hpp"
#include "oatpp/base/Log.hpp"

namespace oatpp { namespace sqlserver {

Executor::Executor(const std::shared_ptr<provider::Provider<Connection>>& connectionProvider)
  : m_connectionProvider(connectionProvider)
  , m_resultMapper(std::make_shared<mapping::ResultMapper>())
{
}

std::shared_ptr<data::mapping::TypeResolver> Executor::createTypeResolver() {
  return std::make_shared<data::mapping::TypeResolver>();
}

data::share::StringTemplate Executor::parseQueryTemplate(const oatpp::String& name,
                                                         const oatpp::String& text,
                                                         const ParamsTypeMap& paramsTypeMap,
                                                         bool prepare)
{
  (void) name;
  (void) paramsTypeMap;
  (void) prepare;
  data::share::StringTemplate result(text);
  return result;
}

provider::ResourceHandle<orm::Connection> Executor::getConnection() {
  return m_connectionProvider->get();
}

std::shared_ptr<orm::QueryResult> Executor::execute(const StringTemplate& queryTemplate,
                                                    const std::unordered_map<oatpp::String, oatpp::Void>& params,
                                                    const std::shared_ptr<const data::mapping::TypeResolver>& typeResolver,
                                                    const provider::ResourceHandle<orm::Connection>& connection)
{
  (void) params;
  
  auto conn = std::static_pointer_cast<Connection>(connection.object);
  
  // Basic query execution - this is a simplified implementation
  HSTMT stmt;
  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, conn->getHandle(), &stmt);
  
  if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
    throw std::runtime_error("Failed to allocate statement handle");
  }
  
  ret = SQLExecDirect(stmt, (SQLCHAR*)queryTemplate.text->c_str(), SQL_NTS);
  
  return std::make_shared<QueryResult>(stmt, connection, m_resultMapper, typeResolver);
}

std::shared_ptr<orm::QueryResult> Executor::begin(const provider::ResourceHandle<orm::Connection>& connection) {
  auto conn = connection ? connection : getConnection();
  return execute(data::share::StringTemplate("BEGIN TRANSACTION"), {}, createTypeResolver(), conn);
}

std::shared_ptr<orm::QueryResult> Executor::commit(const provider::ResourceHandle<orm::Connection>& connection) {
  return execute(data::share::StringTemplate("COMMIT"), {}, createTypeResolver(), connection);
}

std::shared_ptr<orm::QueryResult> Executor::rollback(const provider::ResourceHandle<orm::Connection>& connection) {
  return execute(data::share::StringTemplate("ROLLBACK"), {}, createTypeResolver(), connection);
}

v_int64 Executor::getSchemaVersion(const oatpp::String& suffix,
                                   const provider::ResourceHandle<orm::Connection>& connection) {
  (void) suffix;
  (void) connection;
  return 0; // Simplified implementation
}

void Executor::migrateSchema(const oatpp::String& script,
                            v_int64 newVersion,
                            const oatpp::String& suffix,
                            const provider::ResourceHandle<orm::Connection>& connection) {
  (void) script;
  (void) newVersion;
  (void) suffix;
  (void) connection;
  // Simplified implementation - no migration support
}

}}