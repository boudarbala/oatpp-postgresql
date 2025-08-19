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

#ifndef oatpp_sqlserver_mapping_type_Decimal_hpp
#define oatpp_sqlserver_mapping_type_Decimal_hpp

#include "oatpp/Types.hpp"
#include <string>
#include <cstdint>

namespace oatpp { namespace sqlserver { namespace mapping { namespace type {

namespace __class {
  class Decimal;
}

/**
 * High-precision decimal number implementation for SQL Server DECIMAL/NUMERIC types.
 * Supports precision up to 38 digits and scale from 0 to precision.
 */
class DecimalObject {
public:
  static constexpr v_int32 MAX_PRECISION = 38;
  static constexpr v_int32 MAX_SCALE = 38;

private:
  std::string m_value;
  v_int32 m_precision;
  v_int32 m_scale;
  bool m_isNegative;

public:
  /**
   * Constructor from string representation.
   * @param value - string representation of decimal number
   * @param precision - total number of digits (1-38)
   * @param scale - number of digits after decimal point (0-precision)
   */
  DecimalObject(const std::string& value, v_int32 precision = 18, v_int32 scale = 2);

  /**
   * Constructor from double value.
   * @param value - double value to convert
   * @param precision - total number of digits (1-38)
   * @param scale - number of digits after decimal point (0-precision)
   */
  DecimalObject(double value, v_int32 precision = 18, v_int32 scale = 2);

  /**
   * Constructor from integer value.
   * @param value - integer value to convert
   * @param precision - total number of digits (1-38)
   * @param scale - number of digits after decimal point (0-precision)
   */
  DecimalObject(v_int64 value, v_int32 precision = 18, v_int32 scale = 0);

  /**
   * Get string representation of the decimal value.
   * @return string representation
   */
  std::string toString() const;

  /**
   * Get double representation of the decimal value.
   * @return double value (may lose precision)
   */
  double toDouble() const;

  /**
   * Get integer representation of the decimal value.
   * @return integer value (truncates decimal part)
   */
  v_int64 toInt64() const;

  /**
   * Get precision (total number of digits).
   * @return precision
   */
  v_int32 getPrecision() const;

  /**
   * Get scale (number of digits after decimal point).
   * @return scale
   */
  v_int32 getScale() const;

  /**
   * Check if the decimal is negative.
   * @return true if negative
   */
  bool isNegative() const;

  /**
   * Check if the decimal is zero.
   * @return true if zero
   */
  bool isZero() const;

  /**
   * Add another decimal.
   * @param other - decimal to add
   * @return new decimal with the result
   */
  DecimalObject add(const DecimalObject& other) const;

  /**
   * Subtract another decimal.
   * @param other - decimal to subtract
   * @return new decimal with the result
   */
  DecimalObject subtract(const DecimalObject& other) const;

  /**
   * Multiply by another decimal.
   * @param other - decimal to multiply by
   * @return new decimal with the result
   */
  DecimalObject multiply(const DecimalObject& other) const;

  /**
   * Divide by another decimal.
   * @param other - decimal to divide by
   * @return new decimal with the result
   */
  DecimalObject divide(const DecimalObject& other) const;

  // Comparison operators
  bool operator==(const DecimalObject& other) const;
  bool operator!=(const DecimalObject& other) const;
  bool operator<(const DecimalObject& other) const;
  bool operator<=(const DecimalObject& other) const;
  bool operator>(const DecimalObject& other) const;
  bool operator>=(const DecimalObject& other) const;

private:
  void normalize();
  std::string addStrings(const std::string& a, const std::string& b) const;
  std::string subtractStrings(const std::string& a, const std::string& b) const;
  int compareStrings(const std::string& a, const std::string& b) const;
};

/**
 * Decimal type for SQL Server DECIMAL/NUMERIC columns.
 */
typedef oatpp::data::type::Primitive<DecimalObject, __class::Decimal> Decimal;

namespace __class {

class Decimal {
public:

  class Inter : public oatpp::Type::Interpretation<type::Decimal, oatpp::String> {
  public:

    oatpp::String interpret(const type::Decimal& value) const override {
      if (value) {
        return oatpp::String(value->toString());
      }
      return nullptr;
    }

    type::Decimal reproduce(const oatpp::String& value) const override {
      if (value) {
        return std::make_shared<DecimalObject>(value->c_str());
      }
      return nullptr;
    }

  };

private:
  static oatpp::Type* createType();
public:

  static const oatpp::ClassId CLASS_ID;
  static oatpp::Type* getType();

};

}

}}}}

#endif // oatpp_sqlserver_mapping_type_Decimal_hpp