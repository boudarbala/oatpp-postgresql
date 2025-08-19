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

#ifndef oatpp_sqlserver_mapping_ResultMapper_hpp
#define oatpp_sqlserver_mapping_ResultMapper_hpp

#include "Deserializer.hpp"
#include "oatpp/data/mapping/TypeResolver.hpp"
#include "oatpp/Types.hpp"
#include <sql.h>
#include <sqlext.h>
#include <unordered_map>

namespace oatpp { namespace sqlserver { namespace mapping {

/**
 * Mapper from SQL Server result to oatpp objects.
 */
class ResultMapper {
public:

  /**
   * Result data.
   */
  struct ResultData {

    /**
     * Constructor.
     * @param stmt - SQL Server statement handle
     * @param pTypeResolver - type resolver
     */
    ResultData(HSTMT stmt, const std::shared_ptr<const data::mapping::TypeResolver>& pTypeResolver);

    HSTMT statement;
    std::shared_ptr<const data::mapping::TypeResolver> typeResolver;

    v_int32 rowIndex;
    v_int32 colCount;

    std::vector<oatpp::String> colNames;
    std::unordered_map<oatpp::String, v_int32> colIndices;

  };

private:
  Deserializer m_deserializer;

public:

  /**
   * Constructor.
   */
  ResultMapper();

  /**
   * Read one row as collection.
   */
  oatpp::Void readOneRowAsCollection(const ResultData* resultData,
                                    const oatpp::Type* type,
                                    v_int32 rowIndex);

  /**
   * Read one row as map.
   */
  oatpp::Void readOneRowAsMap(const ResultData* resultData,
                             const oatpp::Type* type,
                             v_int32 rowIndex);

  /**
   * Read one row as object.
   */
  oatpp::Void readOneRowAsObject(const ResultData* resultData,
                                const oatpp::Type* type,
                                v_int32 rowIndex);

  /**
   * Read one row.
   */
  oatpp::Void readOneRow(const ResultData* resultData,
                        const oatpp::Type* type,
                        v_int32 rowIndex);

  /**
   * Read rows.
   */
  oatpp::Void readRows(const ResultData* resultData,
                      const oatpp::Type* type,
                      v_int32 count,
                      v_int32 startRow);

};

}}}

#endif // oatpp_sqlserver_mapping_ResultMapper_hpp