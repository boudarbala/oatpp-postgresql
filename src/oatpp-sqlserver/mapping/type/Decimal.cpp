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

#include "Decimal.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <stdexcept>

namespace oatpp { namespace sqlserver { namespace mapping { namespace type {

DecimalObject::DecimalObject(const std::string& value, v_int32 precision, v_int32 scale)
  : m_precision(precision), m_scale(scale), m_isNegative(false) {
  
  if (precision < 1 || precision > MAX_PRECISION) {
    throw std::invalid_argument("Precision must be between 1 and " + std::to_string(MAX_PRECISION));
  }
  if (scale < 0 || scale > precision) {
    throw std::invalid_argument("Scale must be between 0 and precision");
  }

  std::string cleanValue = value;
  
  // Handle negative sign
  if (!cleanValue.empty() && cleanValue[0] == '-') {
    m_isNegative = true;
    cleanValue = cleanValue.substr(1);
  }
  
  // Remove leading zeros
  size_t firstNonZero = cleanValue.find_first_not_of('0');
  if (firstNonZero != std::string::npos) {
    cleanValue = cleanValue.substr(firstNonZero);
  } else {
    cleanValue = "0";
    m_isNegative = false; // Zero is not negative
  }
  
  // Find decimal point
  size_t decimalPos = cleanValue.find('.');
  if (decimalPos == std::string::npos) {
    m_value = cleanValue + std::string(scale, '0');
  } else {
    std::string integerPart = cleanValue.substr(0, decimalPos);
    std::string fractionalPart = cleanValue.substr(decimalPos + 1);
    
    // Pad or truncate fractional part to match scale
    if (fractionalPart.length() < static_cast<size_t>(scale)) {
      fractionalPart += std::string(scale - fractionalPart.length(), '0');
    } else if (fractionalPart.length() > static_cast<size_t>(scale)) {
      fractionalPart = fractionalPart.substr(0, scale);
    }
    
    m_value = integerPart + fractionalPart;
  }
  
  normalize();
}

DecimalObject::DecimalObject(double value, v_int32 precision, v_int32 scale)
  : m_precision(precision), m_scale(scale) {
  
  if (precision < 1 || precision > MAX_PRECISION) {
    throw std::invalid_argument("Precision must be between 1 and " + std::to_string(MAX_PRECISION));
  }
  if (scale < 0 || scale > precision) {
    throw std::invalid_argument("Scale must be between 0 and precision");
  }

  std::ostringstream oss;
  oss << std::fixed << std::setprecision(scale) << value;
  
  *this = DecimalObject(oss.str(), precision, scale);
}

DecimalObject::DecimalObject(v_int64 value, v_int32 precision, v_int32 scale)
  : m_precision(precision), m_scale(scale) {
  
  if (precision < 1 || precision > MAX_PRECISION) {
    throw std::invalid_argument("Precision must be between 1 and " + std::to_string(MAX_PRECISION));
  }
  if (scale < 0 || scale > precision) {
    throw std::invalid_argument("Scale must be between 0 and precision");
  }

  *this = DecimalObject(std::to_string(value), precision, scale);
}

void DecimalObject::normalize() {
  // Remove leading zeros
  size_t firstNonZero = m_value.find_first_not_of('0');
  if (firstNonZero != std::string::npos) {
    m_value = m_value.substr(firstNonZero);
  } else {
    m_value = std::string(m_scale, '0');
    if (m_value.empty()) {
      m_value = "0";
    }
    m_isNegative = false;
  }
  
  // Ensure we have enough digits for precision
  if (static_cast<v_int32>(m_value.length()) > m_precision) {
    throw std::overflow_error("Value exceeds specified precision");
  }
}

std::string DecimalObject::toString() const {
  if (m_scale == 0) {
    return (m_isNegative ? "-" : "") + m_value;
  }
  
  std::string result = m_value;
  
  // Pad with leading zeros if necessary
  while (static_cast<v_int32>(result.length()) < m_scale + 1) {
    result = "0" + result;
  }
  
  // Insert decimal point
  size_t decimalPos = result.length() - m_scale;
  result.insert(decimalPos, ".");
  
  return (m_isNegative ? "-" : "") + result;
}

double DecimalObject::toDouble() const {
  return std::stod(toString());
}

v_int64 DecimalObject::toInt64() const {
  if (m_scale == 0) {
    v_int64 result = std::stoll(m_value);
    return m_isNegative ? -result : result;
  }
  
  // Extract integer part
  std::string integerPart = m_value;
  if (static_cast<v_int32>(integerPart.length()) > m_scale) {
    integerPart = integerPart.substr(0, integerPart.length() - m_scale);
  } else {
    integerPart = "0";
  }
  
  v_int64 result = std::stoll(integerPart);
  return m_isNegative ? -result : result;
}

v_int32 DecimalObject::getPrecision() const {
  return m_precision;
}

v_int32 DecimalObject::getScale() const {
  return m_scale;
}

bool DecimalObject::isNegative() const {
  return m_isNegative;
}

bool DecimalObject::isZero() const {
  return m_value.find_first_not_of('0') == std::string::npos;
}

DecimalObject DecimalObject::add(const DecimalObject& other) const {
  // For simplicity, use double arithmetic (real implementation would use arbitrary precision)
  double result = toDouble() + other.toDouble();
  v_int32 newPrecision = std::max(m_precision, other.m_precision);
  v_int32 newScale = std::max(m_scale, other.m_scale);
  return DecimalObject(result, newPrecision, newScale);
}

DecimalObject DecimalObject::subtract(const DecimalObject& other) const {
  double result = toDouble() - other.toDouble();
  v_int32 newPrecision = std::max(m_precision, other.m_precision);
  v_int32 newScale = std::max(m_scale, other.m_scale);
  return DecimalObject(result, newPrecision, newScale);
}

DecimalObject DecimalObject::multiply(const DecimalObject& other) const {
  double result = toDouble() * other.toDouble();
  v_int32 newPrecision = std::min(MAX_PRECISION, m_precision + other.m_precision);
  v_int32 newScale = std::min(newPrecision, m_scale + other.m_scale);
  return DecimalObject(result, newPrecision, newScale);
}

DecimalObject DecimalObject::divide(const DecimalObject& other) const {
  if (other.isZero()) {
    throw std::runtime_error("Division by zero");
  }
  double result = toDouble() / other.toDouble();
  return DecimalObject(result, m_precision, m_scale);
}

bool DecimalObject::operator==(const DecimalObject& other) const {
  // Normalize both values to same scale for comparison
  v_int32 maxScale = std::max(m_scale, other.m_scale);
  DecimalObject a(*this);
  DecimalObject b(other);
  
  // Adjust scales if needed (simplified comparison)
  return a.toString() == b.toString();
}

bool DecimalObject::operator!=(const DecimalObject& other) const {
  return !(*this == other);
}

bool DecimalObject::operator<(const DecimalObject& other) const {
  return toDouble() < other.toDouble();
}

bool DecimalObject::operator<=(const DecimalObject& other) const {
  return toDouble() <= other.toDouble();
}

bool DecimalObject::operator>(const DecimalObject& other) const {
  return toDouble() > other.toDouble();
}

bool DecimalObject::operator>=(const DecimalObject& other) const {
  return toDouble() >= other.toDouble();
}

namespace __class {

const oatpp::ClassId Decimal::CLASS_ID("oatpp::sqlserver::mapping::type::Decimal");

oatpp::Type* Decimal::createType() {
  Type::Info info;
  info.interpretationMap = {
    {"oatpp::String", new Inter()}
  };
  return new oatpp::Type(CLASS_ID, info);
}

oatpp::Type* Decimal::getType() {
  static Type* type = createType();
  return type;
}

}

}}}}