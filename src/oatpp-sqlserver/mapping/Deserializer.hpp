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

#ifndef oatpp_sqlserver_mapping_Deserializer_hpp
#define oatpp_sqlserver_mapping_Deserializer_hpp

#include "oatpp/data/mapping/TypeResolver.hpp"
#include "oatpp/Types.hpp"

#include <sql.h>
#include <sqlext.h>

namespace oatpp { namespace sqlserver { namespace mapping {

/**
 * Mapper from SQL Server values to oatpp values.
 */
class Deserializer {
public:

  struct InData {
    std::shared_ptr<const data::mapping::TypeResolver> typeResolver;
    SQLSMALLINT sqlType;
    const char* data;
    v_buff_size size;
    bool isNull;
  };

public:
  typedef oatpp::Void (*DeserializerMethod)(const Deserializer*, const InData&, const Type*);

private:
  std::vector<DeserializerMethod> m_methods;

public:
  Deserializer();

  void setDeserializerMethod(const data::type::ClassId& classId, DeserializerMethod method);
  void setDeserializerMethods(const std::vector<DeserializerMethod>& methods);

  oatpp::Void deserialize(const InData& data, const Type* type) const;

  static std::vector<DeserializerMethod> createDeserializerMethods();

private:
  static oatpp::Void deserializeString(const Deserializer* _this, const InData& data, const Type* type);
  static oatpp::Void deserializeInt32(const Deserializer* _this, const InData& data, const Type* type);
  static oatpp::Void deserializeInt64(const Deserializer* _this, const InData& data, const Type* type);
  static oatpp::Void deserializeFloat32(const Deserializer* _this, const InData& data, const Type* type);
  static oatpp::Void deserializeFloat64(const Deserializer* _this, const InData& data, const Type* type);
  static oatpp::Void deserializeBoolean(const Deserializer* _this, const InData& data, const Type* type);

};

}}}

#endif // oatpp_sqlserver_mapping_Deserializer_hpp