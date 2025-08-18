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

#include "ConnectionProvider.hpp"

namespace oatpp { namespace sqlserver {

void ConnectionProvider::ConnectionInvalidator::invalidate(const std::shared_ptr<Connection> &resource) {
  (void) resource;
  //Do nothing.
}

ConnectionProvider::ConnectionProvider(const oatpp::String& connectionString)
  : m_invalidator(std::make_shared<ConnectionInvalidator>())
  , m_connectionString(connectionString)
{}

provider::ResourceHandle<Connection> ConnectionProvider::get() {

  HENV henv;
  HDBC hdbc;
  SQLRETURN ret;

  // Allocate environment handle
  ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
  if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
    throw std::runtime_error("[oatpp::sqlserver::ConnectionProvider::get()]: "
                             "Error. Failed to allocate environment handle.");
  }

  // Set the ODBC version environment attribute
  ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
  if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
    SQLFreeHandle(SQL_HANDLE_ENV, henv);
    throw std::runtime_error("[oatpp::sqlserver::ConnectionProvider::get()]: "
                             "Error. Failed to set ODBC version.");
  }

  // Allocate connection handle
  ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
  if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
    SQLFreeHandle(SQL_HANDLE_ENV, henv);
    throw std::runtime_error("[oatpp::sqlserver::ConnectionProvider::get()]: "
                             "Error. Failed to allocate connection handle.");
  }

  // Connect to SQL Server
  ret = SQLDriverConnect(hdbc, NULL, 
                         (SQLCHAR*)m_connectionString->c_str(), SQL_NTS,
                         NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
  if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
    SQLCHAR sqlState[6], message[256];
    SQLINTEGER nativeError;
    SQLSMALLINT messageLength;
    
    SQLGetDiagRec(SQL_HANDLE_DBC, hdbc, 1, sqlState, &nativeError, 
                  message, sizeof(message), &messageLength);
    
    std::string errMsg = std::string((char*)message, messageLength);
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
    SQLFreeHandle(SQL_HANDLE_ENV, henv);
    throw std::runtime_error("[oatpp::sqlserver::ConnectionProvider::get()]: "
                             "Error. Can't connect. " + errMsg);
  }

  return provider::ResourceHandle<Connection>(std::make_shared<ConnectionImpl>(henv, hdbc), m_invalidator);

}

async::CoroutineStarterForResult<const provider::ResourceHandle<Connection>&> ConnectionProvider::getAsync() {
  throw std::runtime_error("[oatpp::sqlserver::ConnectionProvider::getAsync()]: Error. Not implemented!");
}

void ConnectionProvider::stop() {
  // DO nothing
}

}}
