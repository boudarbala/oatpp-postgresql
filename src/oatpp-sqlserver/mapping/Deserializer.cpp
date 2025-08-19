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

#include "Deserializer.hpp"

namespace oatpp { namespace sqlserver { namespace mapping {

Deserializer::Deserializer() {
  m_methods.resize(data::type::ClassId::getClassCount(), nullptr);
}

void Deserializer::setDeserializerMethod(const data::type::ClassId& classId, DeserializerMethod method) {
  const auto id = classId.id;
  if(id >= m_methods.size()) {
    m_methods.resize(id + 1, nullptr);
  }
  m_methods[id] = method;
}

void Deserializer::setDeserializerMethods(const std::vector<DeserializerMethod>& methods) {
  m_methods = methods;
}

oatpp::Void Deserializer::deserialize(const InData& data, const Type* type) const {
  auto id = type->classId.id;
  if(id < m_methods.size()) {
    auto method = m_methods[id];
    if(method) {
      return (*method)(this, data, type);
    }
  }
  
  throw std::runtime_error("[oatpp::sqlserver::mapping::Deserializer::deserialize()]: "
                           "Error. No deserialize method for type '" + std::string(type->classId.name) + "'");
}

std::vector<Deserializer::DeserializerMethod> Deserializer::createDeserializerMethods() {
  std::vector<DeserializerMethod> methods;
  methods.resize(data::type::ClassId::getClassCount(), nullptr);
  
  methods[String::Class::CLASS_ID.id] = &Deserializer::deserializeString;
  methods[Int32::Class::CLASS_ID.id] = &Deserializer::deserializeInt32;
  methods[Int64::Class::CLASS_ID.id] = &Deserializer::deserializeInt64;
  methods[Float32::Class::CLASS_ID.id] = &Deserializer::deserializeFloat32;
  methods[Float64::Class::CLASS_ID.id] = &Deserializer::deserializeFloat64;
  methods[Boolean::Class::CLASS_ID.id] = &Deserializer::deserializeBoolean;
  
  return methods;
}

oatpp::Void Deserializer::deserializeString(const Deserializer* _this, const InData& data, const Type* type) {
  (void) _this;
  (void) type;
  
  if(data.isNull) {
    return String();
  }
  
  return String((const char*)data.data, data.size);
}

oatpp::Void Deserializer::deserializeInt32(const Deserializer* _this, const InData& data, const Type* type) {
  (void) _this;
  (void) type;
  
  if(data.isNull) {
    return Int32();
  }
  
  return Int32(*((int32_t*)data.data));
}

oatpp::Void Deserializer::deserializeInt64(const Deserializer* _this, const InData& data, const Type* type) {
  (void) _this;
  (void) type;
  
  if(data.isNull) {
    return Int64();
  }
  
  return Int64(*((int64_t*)data.data));
}

oatpp::Void Deserializer::deserializeFloat32(const Deserializer* _this, const InData& data, const Type* type) {
  (void) _this;
  (void) type;
  
  if(data.isNull) {
    return Float32();
  }
  
  return Float32(*((float*)data.data));
}

oatpp::Void Deserializer::deserializeFloat64(const Deserializer* _this, const InData& data, const Type* type) {
  (void) _this;
  (void) type;
  
  if(data.isNull) {
    return Float64();
  }
  
  return Float64(*((double*)data.data));
}

oatpp::Void Deserializer::deserializeBoolean(const Deserializer* _this, const InData& data, const Type* type) {
  (void) _this;
  (void) type;
  
  if(data.isNull) {
    return Boolean();
  }
  
  return Boolean(*((bool*)data.data));
}

}}}