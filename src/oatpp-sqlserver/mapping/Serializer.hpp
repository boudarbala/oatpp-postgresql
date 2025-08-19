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

#ifndef oatpp_sqlserver_mapping_Serializer_hpp
#define oatpp_sqlserver_mapping_Serializer_hpp

#include "oatpp/Types.hpp"
#include <sql.h>
#include <sqlext.h>

namespace oatpp { namespace sqlserver { namespace mapping {

/**
 * Mapper of oatpp values to SQL Server values.
 */
class Serializer {
public:

  struct OutputData {
    SQLSMALLINT sqlType = SQL_UNKNOWN_TYPE;
    std::unique_ptr<char[]> dataBuffer;
    char* data = nullptr;
    int dataSize = -1;
  };

public:
  typedef OutputData (*SerializerMethod)(const Serializer*, const oatpp::Void&, const Type*);

private:
  std::vector<SerializerMethod> m_methods;

public:
  Serializer();

  void setSerializerMethod(const data::type::ClassId& classId, SerializerMethod method);

  OutputData serialize(const oatpp::Void& polymorph, const Type* type) const;

private:
  static OutputData serializeString(const Serializer* _this, const oatpp::Void& polymorph, const Type* type);
  static OutputData serializeInt32(const Serializer* _this, const oatpp::Void& polymorph, const Type* type);
  static OutputData serializeInt64(const Serializer* _this, const oatpp::Void& polymorph, const Type* type);
  static OutputData serializeFloat32(const Serializer* _this, const oatpp::Void& polymorph, const Type* type);
  static OutputData serializeFloat64(const Serializer* _this, const oatpp::Void& polymorph, const Type* type);
  static OutputData serializeBoolean(const Serializer* _this, const oatpp::Void& polymorph, const Type* type);

};

}}}

#endif // oatpp_sqlserver_mapping_Serializer_hpp