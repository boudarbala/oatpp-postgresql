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

#ifndef oatpp_sqlserver_mapping_type_DateTime_hpp
#define oatpp_sqlserver_mapping_type_DateTime_hpp

#include "oatpp/Types.hpp"
#include <chrono>
#include <string>

namespace oatpp { namespace sqlserver { namespace mapping { namespace type {

namespace __class {
  class DateTime2;
  class DateTimeOffset;
  class Time;
  class Date;
}

/**
 * High-precision DateTime2 implementation for SQL Server DATETIME2 type.
 * Supports fractional seconds precision from 0 to 7.
 */
class DateTime2Object {
public:
  static constexpr v_int32 MAX_FRACTIONAL_PRECISION = 7;

private:
  std::chrono::system_clock::time_point m_timePoint;
  v_int32 m_fractionalPrecision;

public:
  /**
   * Constructor from time point.
   * @param timePoint - system clock time point
   * @param fractionalPrecision - precision of fractional seconds (0-7)
   */
  DateTime2Object(const std::chrono::system_clock::time_point& timePoint, v_int32 fractionalPrecision = 7);

  /**
   * Constructor from ISO 8601 string.
   * @param isoString - ISO 8601 formatted string
   * @param fractionalPrecision - precision of fractional seconds (0-7)
   */
  DateTime2Object(const std::string& isoString, v_int32 fractionalPrecision = 7);

  /**
   * Constructor from components.
   * @param year - year (1-9999)
   * @param month - month (1-12)
   * @param day - day (1-31)
   * @param hour - hour (0-23)
   * @param minute - minute (0-59)
   * @param second - second (0-59)
   * @param nanosecond - nanosecond (0-999999999)
   * @param fractionalPrecision - precision of fractional seconds (0-7)
   */
  DateTime2Object(v_int32 year, v_int32 month, v_int32 day,
                  v_int32 hour = 0, v_int32 minute = 0, v_int32 second = 0,
                  v_int32 nanosecond = 0, v_int32 fractionalPrecision = 7);

  /**
   * Get the time point.
   * @return system clock time point
   */
  std::chrono::system_clock::time_point getTimePoint() const;

  /**
   * Get fractional precision.
   * @return fractional precision (0-7)
   */
  v_int32 getFractionalPrecision() const;

  /**
   * Convert to ISO 8601 string.
   * @return ISO 8601 formatted string
   */
  std::string toISOString() const;

  /**
   * Convert to SQL Server DATETIME2 string format.
   * @return SQL Server formatted string
   */
  std::string toSqlServerString() const;

  /**
   * Get year component.
   * @return year (1-9999)
   */
  v_int32 getYear() const;

  /**
   * Get month component.
   * @return month (1-12)
   */
  v_int32 getMonth() const;

  /**
   * Get day component.
   * @return day (1-31)
   */
  v_int32 getDay() const;

  /**
   * Get hour component.
   * @return hour (0-23)
   */
  v_int32 getHour() const;

  /**
   * Get minute component.
   * @return minute (0-59)
   */
  v_int32 getMinute() const;

  /**
   * Get second component.
   * @return second (0-59)
   */
  v_int32 getSecond() const;

  /**
   * Get nanosecond component.
   * @return nanosecond (0-999999999)
   */
  v_int32 getNanosecond() const;

  /**
   * Add duration to the datetime.
   * @param duration - duration to add
   * @return new DateTime2Object
   */
  DateTime2Object add(const std::chrono::nanoseconds& duration) const;

  /**
   * Subtract duration from the datetime.
   * @param duration - duration to subtract
   * @return new DateTime2Object
   */
  DateTime2Object subtract(const std::chrono::nanoseconds& duration) const;

  // Comparison operators
  bool operator==(const DateTime2Object& other) const;
  bool operator!=(const DateTime2Object& other) const;
  bool operator<(const DateTime2Object& other) const;
  bool operator<=(const DateTime2Object& other) const;
  bool operator>(const DateTime2Object& other) const;
  bool operator>=(const DateTime2Object& other) const;

private:
  void validatePrecision(v_int32 precision);
  std::chrono::system_clock::time_point parseISOString(const std::string& isoString);
};

/**
 * DateTimeOffset implementation for SQL Server DATETIMEOFFSET type.
 * Includes timezone offset information.
 */
class DateTimeOffsetObject {
private:
  DateTime2Object m_dateTime;
  std::chrono::minutes m_offsetMinutes;

public:
  /**
   * Constructor from DateTime2 and offset.
   * @param dateTime - DateTime2 object
   * @param offsetMinutes - timezone offset in minutes
   */
  DateTimeOffsetObject(const DateTime2Object& dateTime, const std::chrono::minutes& offsetMinutes);

  /**
   * Constructor from ISO 8601 string with timezone.
   * @param isoString - ISO 8601 formatted string with timezone
   * @param fractionalPrecision - precision of fractional seconds (0-7)
   */
  DateTimeOffsetObject(const std::string& isoString, v_int32 fractionalPrecision = 7);

  /**
   * Get the DateTime2 component.
   * @return DateTime2Object
   */
  DateTime2Object getDateTime() const;

  /**
   * Get the timezone offset.
   * @return offset in minutes
   */
  std::chrono::minutes getOffset() const;

  /**
   * Convert to UTC DateTime2.
   * @return UTC DateTime2Object
   */
  DateTime2Object toUTC() const;

  /**
   * Convert to ISO 8601 string with timezone.
   * @return ISO 8601 formatted string
   */
  std::string toISOString() const;

  /**
   * Convert to SQL Server DATETIMEOFFSET string format.
   * @return SQL Server formatted string
   */
  std::string toSqlServerString() const;

  // Comparison operators
  bool operator==(const DateTimeOffsetObject& other) const;
  bool operator!=(const DateTimeOffsetObject& other) const;
  bool operator<(const DateTimeOffsetObject& other) const;
  bool operator<=(const DateTimeOffsetObject& other) const;
  bool operator>(const DateTimeOffsetObject& other) const;
  bool operator>=(const DateTimeOffsetObject& other) const;
};

/**
 * Time implementation for SQL Server TIME type.
 * Represents time of day with fractional seconds.
 */
class TimeObject {
private:
  std::chrono::nanoseconds m_timeOfDay;
  v_int32 m_fractionalPrecision;

public:
  /**
   * Constructor from time components.
   * @param hour - hour (0-23)
   * @param minute - minute (0-59)
   * @param second - second (0-59)
   * @param nanosecond - nanosecond (0-999999999)
   * @param fractionalPrecision - precision of fractional seconds (0-7)
   */
  TimeObject(v_int32 hour, v_int32 minute, v_int32 second = 0,
             v_int32 nanosecond = 0, v_int32 fractionalPrecision = 7);

  /**
   * Constructor from string.
   * @param timeString - time string in HH:MM:SS.nnnnnnn format
   * @param fractionalPrecision - precision of fractional seconds (0-7)
   */
  TimeObject(const std::string& timeString, v_int32 fractionalPrecision = 7);

  /**
   * Get hour component.
   * @return hour (0-23)
   */
  v_int32 getHour() const;

  /**
   * Get minute component.
   * @return minute (0-59)
   */
  v_int32 getMinute() const;

  /**
   * Get second component.
   * @return second (0-59)
   */
  v_int32 getSecond() const;

  /**
   * Get nanosecond component.
   * @return nanosecond (0-999999999)
   */
  v_int32 getNanosecond() const;

  /**
   * Get fractional precision.
   * @return fractional precision (0-7)
   */
  v_int32 getFractionalPrecision() const;

  /**
   * Convert to string.
   * @return time string in HH:MM:SS.nnnnnnn format
   */
  std::string toString() const;

  /**
   * Get total nanoseconds since midnight.
   * @return nanoseconds since midnight
   */
  std::chrono::nanoseconds getTimeOfDay() const;

  // Comparison operators
  bool operator==(const TimeObject& other) const;
  bool operator!=(const TimeObject& other) const;
  bool operator<(const TimeObject& other) const;
  bool operator<=(const TimeObject& other) const;
  bool operator>(const TimeObject& other) const;
  bool operator>=(const TimeObject& other) const;
};

/**
 * Date implementation for SQL Server DATE type.
 * Represents date without time component.
 */
class DateObject {
private:
  v_int32 m_year;
  v_int32 m_month;
  v_int32 m_day;

public:
  /**
   * Constructor from date components.
   * @param year - year (1-9999)
   * @param month - month (1-12)
   * @param day - day (1-31)
   */
  DateObject(v_int32 year, v_int32 month, v_int32 day);

  /**
   * Constructor from string.
   * @param dateString - date string in YYYY-MM-DD format
   */
  DateObject(const std::string& dateString);

  /**
   * Get year component.
   * @return year (1-9999)
   */
  v_int32 getYear() const;

  /**
   * Get month component.
   * @return month (1-12)
   */
  v_int32 getMonth() const;

  /**
   * Get day component.
   * @return day (1-31)
   */
  v_int32 getDay() const;

  /**
   * Convert to string.
   * @return date string in YYYY-MM-DD format
   */
  std::string toString() const;

  /**
   * Get day of week.
   * @return day of week (0=Sunday, 6=Saturday)
   */
  v_int32 getDayOfWeek() const;

  /**
   * Get day of year.
   * @return day of year (1-366)
   */
  v_int32 getDayOfYear() const;

  /**
   * Add days to the date.
   * @param days - number of days to add
   * @return new DateObject
   */
  DateObject addDays(v_int32 days) const;

  // Comparison operators
  bool operator==(const DateObject& other) const;
  bool operator!=(const DateObject& other) const;
  bool operator<(const DateObject& other) const;
  bool operator<=(const DateObject& other) const;
  bool operator>(const DateObject& other) const;
  bool operator>=(const DateObject& other) const;

private:
  void validateDate(v_int32 year, v_int32 month, v_int32 day);
  bool isLeapYear(v_int32 year) const;
  v_int32 getDaysInMonth(v_int32 year, v_int32 month) const;
};

// Type definitions
typedef oatpp::data::type::Primitive<DateTime2Object, __class::DateTime2> DateTime2;
typedef oatpp::data::type::Primitive<DateTimeOffsetObject, __class::DateTimeOffset> DateTimeOffset;
typedef oatpp::data::type::Primitive<TimeObject, __class::Time> Time;
typedef oatpp::data::type::Primitive<DateObject, __class::Date> Date;

namespace __class {

class DateTime2 {
public:
  class Inter : public oatpp::Type::Interpretation<type::DateTime2, oatpp::String> {
  public:
    oatpp::String interpret(const type::DateTime2& value) const override {
      if (value) {
        return oatpp::String(value->toISOString());
      }
      return nullptr;
    }

    type::DateTime2 reproduce(const oatpp::String& value) const override {
      if (value) {
        return std::make_shared<DateTime2Object>(value->c_str());
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

class DateTimeOffset {
public:
  class Inter : public oatpp::Type::Interpretation<type::DateTimeOffset, oatpp::String> {
  public:
    oatpp::String interpret(const type::DateTimeOffset& value) const override {
      if (value) {
        return oatpp::String(value->toISOString());
      }
      return nullptr;
    }

    type::DateTimeOffset reproduce(const oatpp::String& value) const override {
      if (value) {
        return std::make_shared<DateTimeOffsetObject>(value->c_str());
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

class Time {
public:
  class Inter : public oatpp::Type::Interpretation<type::Time, oatpp::String> {
  public:
    oatpp::String interpret(const type::Time& value) const override {
      if (value) {
        return oatpp::String(value->toString());
      }
      return nullptr;
    }

    type::Time reproduce(const oatpp::String& value) const override {
      if (value) {
        return std::make_shared<TimeObject>(value->c_str());
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

class Date {
public:
  class Inter : public oatpp::Type::Interpretation<type::Date, oatpp::String> {
  public:
    oatpp::String interpret(const type::Date& value) const override {
      if (value) {
        return oatpp::String(value->toString());
      }
      return nullptr;
    }

    type::Date reproduce(const oatpp::String& value) const override {
      if (value) {
        return std::make_shared<DateObject>(value->c_str());
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

#endif // oatpp_sqlserver_mapping_type_DateTime_hpp