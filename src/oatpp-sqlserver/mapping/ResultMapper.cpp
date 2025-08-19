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

#include "ResultMapper.hpp"
#include "oatpp/base/Log.hpp"

namespace oatpp { namespace sqlserver { namespace mapping {

ResultMapper::ResultData::ResultData(HSTMT stmt, const std::shared_ptr<const data::mapping::TypeResolver>& pTypeResolver)
  : statement(stmt)
  , typeResolver(pTypeResolver)
  , rowIndex(0)
  , colCount(0)
{
  // Get column count for SQL Server
  SQLSMALLINT numCols;
  SQLRETURN ret = SQLNumResultCols(stmt, &numCols);
  if (ret == SQL_SUCCESS) {
    colCount = numCols;
    
    // Get column information
    for (SQLSMALLINT i = 1; i <= colCount; i++) {
      SQLCHAR colName[256];
      SQLSMALLINT nameLen;
      ret = SQLColAttribute(stmt, i, SQL_DESC_NAME, colName, sizeof(colName), &nameLen, NULL);
      if (ret == SQL_SUCCESS) {
        oatpp::String name = (const char*)colName;
        colNames.push_back(name);
        colIndices.insert({name, i - 1}); // 0-based index for internal use
      }
    }
  }
}

ResultMapper::ResultMapper() {
  m_deserializer.setDeserializerMethods(Deserializer::createDeserializerMethods());
}

oatpp::Void ResultMapper::readOneRowAsCollection(const ResultData* resultData,
                                                 const oatpp::Type* type,
                                                 v_int32 rowIndex) {
  (void) resultData;
  (void) type;
  (void) rowIndex;
  throw std::runtime_error("ResultMapper::readOneRowAsCollection not implemented in simplified version");
}

oatpp::Void ResultMapper::readOneRowAsMap(const ResultData* resultData,
                                         const oatpp::Type* type,
                                         v_int32 rowIndex) {
  (void) resultData;
  (void) type;
  (void) rowIndex;
  throw std::runtime_error("ResultMapper::readOneRowAsMap not implemented in simplified version");
}

oatpp::Void ResultMapper::readOneRowAsObject(const ResultData* resultData,
                                            const oatpp::Type* type,
                                            v_int32 rowIndex) {
  (void) resultData;
  (void) type;
  (void) rowIndex;
  throw std::runtime_error("ResultMapper::readOneRowAsObject not implemented in simplified version");
}

oatpp::Void ResultMapper::readOneRow(const ResultData* resultData,
                                    const oatpp::Type* type,
                                    v_int32 rowIndex) {
  (void) resultData;
  (void) type;
  (void) rowIndex;
  return nullptr; // Simplified implementation
}

oatpp::Void ResultMapper::readRows(const ResultData* resultData,
                                   const oatpp::Type* type,
                                   v_int32 count,
                                   v_int32 startRow) {
  (void) resultData;
  (void) type;
  (void) count;
  (void) startRow;
  return nullptr; // Simplified implementation
}

}}}