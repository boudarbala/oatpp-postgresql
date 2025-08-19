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

#include "Serializer.hpp"

namespace oatpp { namespace sqlserver { namespace mapping {

Serializer::Serializer() {
  m_methods.resize(data::type::ClassId::getClassCount(), nullptr);
  
  // Set up basic serializer methods
  m_methods[String::Class::CLASS_ID.id] = &Serializer::serializeString;
  m_methods[Int32::Class::CLASS_ID.id] = &Serializer::serializeInt32;
  m_methods[Int64::Class::CLASS_ID.id] = &Serializer::serializeInt64;
  m_methods[Float32::Class::CLASS_ID.id] = &Serializer::serializeFloat32;
  m_methods[Float64::Class::CLASS_ID.id] = &Serializer::serializeFloat64;
  m_methods[Boolean::Class::CLASS_ID.id] = &Serializer::serializeBoolean;
}

void Serializer::setSerializerMethod(const data::type::ClassId& classId, SerializerMethod method) {
  const auto id = classId.id;
  if(id >= m_methods.size()) {
    m_methods.resize(id + 1, nullptr);
  }
  m_methods[id] = method;
}

Serializer::OutputData Serializer::serialize(const oatpp::Void& polymorph, const Type* type) const {
  auto id = type->classId.id;
  if(id < m_methods.size()) {
    auto method = m_methods[id];
    if(method) {
      return (*method)(this, polymorph, type);
    }
  }
  
  throw std::runtime_error("[oatpp::sqlserver::mapping::Serializer::serialize()]: "
                           "Error. No serialize method for type '" + std::string(type->classId.name) + "'");
}

Serializer::OutputData Serializer::serializeString(const Serializer* _this, const oatpp::Void& polymorph, const Type* type) {
  (void) _this;
  (void) type;
  
  OutputData result;
  result.sqlType = SQL_VARCHAR;
  
  if(!polymorph) {
    result.data = nullptr;
    result.dataSize = SQL_NULL_DATA;
    return result;
  }
  
  auto str = polymorph.cast<String>();
  if(str) {
    auto size = str->size();
    result.dataBuffer = std::unique_ptr<char[]>(new char[size + 1]);
    memcpy(result.dataBuffer.get(), str->data(), size);
    result.dataBuffer[size] = '\0';
    result.data = result.dataBuffer.get();
    result.dataSize = static_cast<int>(size);
  }
  
  return result;
}

Serializer::OutputData Serializer::serializeInt32(const Serializer* _this, const oatpp::Void& polymorph, const Type* type) {
  (void) _this;
  (void) type;
  
  OutputData result;
  result.sqlType = SQL_INTEGER;
  
  if(!polymorph) {
    result.data = nullptr;
    result.dataSize = SQL_NULL_DATA;
    return result;
  }
  
  auto value = polymorph.cast<Int32>();
  if(value) {
    result.dataBuffer = std::unique_ptr<char[]>(new char[sizeof(int32_t)]);
    *((int32_t*)result.dataBuffer.get()) = *value;
    result.data = result.dataBuffer.get();
    result.dataSize = sizeof(int32_t);
  }
  
  return result;
}

Serializer::OutputData Serializer::serializeInt64(const Serializer* _this, const oatpp::Void& polymorph, const Type* type) {
  (void) _this;
  (void) type;
  
  OutputData result;
  result.sqlType = SQL_BIGINT;
  
  if(!polymorph) {
    result.data = nullptr;
    result.dataSize = SQL_NULL_DATA;
    return result;
  }
  
  auto value = polymorph.cast<Int64>();
  if(value) {
    result.dataBuffer = std::unique_ptr<char[]>(new char[sizeof(int64_t)]);
    *((int64_t*)result.dataBuffer.get()) = *value;
    result.data = result.dataBuffer.get();
    result.dataSize = sizeof(int64_t);
  }
  
  return result;
}

Serializer::OutputData Serializer::serializeFloat32(const Serializer* _this, const oatpp::Void& polymorph, const Type* type) {
  (void) _this;
  (void) type;
  
  OutputData result;
  result.sqlType = SQL_REAL;
  
  if(!polymorph) {
    result.data = nullptr;
    result.dataSize = SQL_NULL_DATA;
    return result;
  }
  
  auto value = polymorph.cast<Float32>();
  if(value) {
    result.dataBuffer = std::unique_ptr<char[]>(new char[sizeof(float)]);
    *((float*)result.dataBuffer.get()) = *value;
    result.data = result.dataBuffer.get();
    result.dataSize = sizeof(float);
  }
  
  return result;
}

Serializer::OutputData Serializer::serializeFloat64(const Serializer* _this, const oatpp::Void& polymorph, const Type* type) {
  (void) _this;
  (void) type;
  
  OutputData result;
  result.sqlType = SQL_DOUBLE;
  
  if(!polymorph) {
    result.data = nullptr;
    result.dataSize = SQL_NULL_DATA;
    return result;
  }
  
  auto value = polymorph.cast<Float64>();
  if(value) {
    result.dataBuffer = std::unique_ptr<char[]>(new char[sizeof(double)]);
    *((double*)result.dataBuffer.get()) = *value;
    result.data = result.dataBuffer.get();
    result.dataSize = sizeof(double);
  }
  
  return result;
}

Serializer::OutputData Serializer::serializeBoolean(const Serializer* _this, const oatpp::Void& polymorph, const Type* type) {
  (void) _this;
  (void) type;
  
  OutputData result;
  result.sqlType = SQL_BIT;
  
  if(!polymorph) {
    result.data = nullptr;
    result.dataSize = SQL_NULL_DATA;
    return result;
  }
  
  auto value = polymorph.cast<Boolean>();
  if(value) {
    result.dataBuffer = std::unique_ptr<char[]>(new char[sizeof(bool)]);
    *((bool*)result.dataBuffer.get()) = *value;
    result.data = result.dataBuffer.get();
    result.dataSize = sizeof(bool);
  }
  
  return result;
}

}}}