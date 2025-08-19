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

#include "DateTime.hpp"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <stdexcept>
#include <regex>

namespace oatpp { namespace sqlserver { namespace mapping { namespace type {

// DateTime2Object Implementation

DateTime2Object::DateTime2Object(const std::chrono::system_clock::time_point& timePoint, v_int32 fractionalPrecision)
  : m_timePoint(timePoint), m_fractionalPrecision(fractionalPrecision) {
  validatePrecision(fractionalPrecision);
}

DateTime2Object::DateTime2Object(const std::string& isoString, v_int32 fractionalPrecision)
  : m_fractionalPrecision(fractionalPrecision) {
  validatePrecision(fractionalPrecision);
  m_timePoint = parseISOString(isoString);
}

DateTime2Object::DateTime2Object(v_int32 year, v_int32 month, v_int32 day,
                                v_int32 hour, v_int32 minute, v_int32 second,
                                v_int32 nanosecond, v_int32 fractionalPrecision)
  : m_fractionalPrecision(fractionalPrecision) {
  validatePrecision(fractionalPrecision);
  
  std::tm tm = {};
  tm.tm_year = year - 1900;
  tm.tm_mon = month - 1;
  tm.tm_mday = day;
  tm.tm_hour = hour;
  tm.tm_min = minute;
  tm.tm_sec = second;
  
  auto timeT = std::mktime(&tm);
  m_timePoint = std::chrono::system_clock::from_time_t(timeT) + 
                std::chrono::nanoseconds(nanosecond);
}

void DateTime2Object::validatePrecision(v_int32 precision) {
  if (precision < 0 || precision > MAX_FRACTIONAL_PRECISION) {
    throw std::invalid_argument("Fractional precision must be between 0 and " + 
                               std::to_string(MAX_FRACTIONAL_PRECISION));
  }
}

std::chrono::system_clock::time_point DateTime2Object::parseISOString(const std::string& isoString) {
  // Parse ISO 8601 format: 2023-12-25T14:30:45.123456789
  std::regex iso_regex(R"((\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})(?:\.(\d+))?(?:Z|[+-]\d{2}:\d{2})?)");
  std::smatch matches;
  
  if (!std::regex_match(isoString, matches, iso_regex)) {
    throw std::invalid_argument("Invalid ISO 8601 datetime format: " + isoString);
  }
  
  std::tm tm = {};
  tm.tm_year = std::stoi(matches[1]) - 1900;
  tm.tm_mon = std::stoi(matches[2]) - 1;
  tm.tm_mday = std::stoi(matches[3]);
  tm.tm_hour = std::stoi(matches[4]);
  tm.tm_min = std::stoi(matches[5]);
  tm.tm_sec = std::stoi(matches[6]);
  
  auto timeT = std::mktime(&tm);
  auto timePoint = std::chrono::system_clock::from_time_t(timeT);
  
  // Add fractional seconds if present
  if (matches[7].matched) {
    std::string fracStr = matches[7];
    // Pad or truncate to nanoseconds
    if (fracStr.length() < 9) {
      fracStr += std::string(9 - fracStr.length(), '0');
    } else if (fracStr.length() > 9) {
      fracStr = fracStr.substr(0, 9);
    }
    auto nanoseconds = std::stoll(fracStr);
    timePoint += std::chrono::nanoseconds(nanoseconds);
  }
  
  return timePoint;
}

std::string DateTime2Object::toISOString() const {
  auto timeT = std::chrono::system_clock::to_time_t(m_timePoint);
  auto tm = *std::gmtime(&timeT);
  
  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
  
  if (m_fractionalPrecision > 0) {
    auto since_epoch = m_timePoint.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(since_epoch);
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(since_epoch - seconds);
    
    // Format fractional part based on precision
    v_int64 fracValue = nanoseconds.count();
    v_int64 divisor = 1;
    for (int i = 0; i < (9 - m_fractionalPrecision); ++i) {
      divisor *= 10;
    }
    fracValue /= divisor;
    
    oss << "." << std::setfill('0') << std::setw(m_fractionalPrecision) << fracValue;
  }
  
  oss << "Z";
  return oss.str();
}

std::string DateTime2Object::toSqlServerString() const {
  auto timeT = std::chrono::system_clock::to_time_t(m_timePoint);
  auto tm = *std::localtime(&timeT);
  
  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
  
  if (m_fractionalPrecision > 0) {
    auto since_epoch = m_timePoint.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(since_epoch);
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(since_epoch - seconds);
    
    v_int64 fracValue = nanoseconds.count();
    v_int64 divisor = 1;
    for (int i = 0; i < (9 - m_fractionalPrecision); ++i) {
      divisor *= 10;
    }
    fracValue /= divisor;
    
    oss << "." << std::setfill('0') << std::setw(m_fractionalPrecision) << fracValue;
  }
  
  return oss.str();
}

std::chrono::system_clock::time_point DateTime2Object::getTimePoint() const {
  return m_timePoint;
}

v_int32 DateTime2Object::getFractionalPrecision() const {
  return m_fractionalPrecision;
}

// Implement other DateTime2Object methods...
v_int32 DateTime2Object::getYear() const {
  auto timeT = std::chrono::system_clock::to_time_t(m_timePoint);
  auto tm = *std::localtime(&timeT);
  return tm.tm_year + 1900;
}

v_int32 DateTime2Object::getMonth() const {
  auto timeT = std::chrono::system_clock::to_time_t(m_timePoint);
  auto tm = *std::localtime(&timeT);
  return tm.tm_mon + 1;
}

v_int32 DateTime2Object::getDay() const {
  auto timeT = std::chrono::system_clock::to_time_t(m_timePoint);
  auto tm = *std::localtime(&timeT);
  return tm.tm_mday;
}

v_int32 DateTime2Object::getHour() const {
  auto timeT = std::chrono::system_clock::to_time_t(m_timePoint);
  auto tm = *std::localtime(&timeT);
  return tm.tm_hour;
}

v_int32 DateTime2Object::getMinute() const {
  auto timeT = std::chrono::system_clock::to_time_t(m_timePoint);
  auto tm = *std::localtime(&timeT);
  return tm.tm_min;
}

v_int32 DateTime2Object::getSecond() const {
  auto timeT = std::chrono::system_clock::to_time_t(m_timePoint);
  auto tm = *std::localtime(&timeT);
  return tm.tm_sec;
}

v_int32 DateTime2Object::getNanosecond() const {
  auto since_epoch = m_timePoint.time_since_epoch();
  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(since_epoch);
  auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(since_epoch - seconds);
  return static_cast<v_int32>(nanoseconds.count());
}

DateTime2Object DateTime2Object::add(const std::chrono::nanoseconds& duration) const {
  return DateTime2Object(m_timePoint + duration, m_fractionalPrecision);
}

DateTime2Object DateTime2Object::subtract(const std::chrono::nanoseconds& duration) const {
  return DateTime2Object(m_timePoint - duration, m_fractionalPrecision);
}

bool DateTime2Object::operator==(const DateTime2Object& other) const {
  return m_timePoint == other.m_timePoint;
}

bool DateTime2Object::operator!=(const DateTime2Object& other) const {
  return !(*this == other);
}

bool DateTime2Object::operator<(const DateTime2Object& other) const {
  return m_timePoint < other.m_timePoint;
}

bool DateTime2Object::operator<=(const DateTime2Object& other) const {
  return m_timePoint <= other.m_timePoint;
}

bool DateTime2Object::operator>(const DateTime2Object& other) const {
  return m_timePoint > other.m_timePoint;
}

bool DateTime2Object::operator>=(const DateTime2Object& other) const {
  return m_timePoint >= other.m_timePoint;
}

// Class implementations for type system
namespace __class {

const oatpp::ClassId DateTime2::CLASS_ID("oatpp::sqlserver::mapping::type::DateTime2");
const oatpp::ClassId DateTimeOffset::CLASS_ID("oatpp::sqlserver::mapping::type::DateTimeOffset");
const oatpp::ClassId Time::CLASS_ID("oatpp::sqlserver::mapping::type::Time");
const oatpp::ClassId Date::CLASS_ID("oatpp::sqlserver::mapping::type::Date");

oatpp::Type* DateTime2::createType() {
  Type::Info info;
  info.interpretationMap = {
    {"oatpp::String", new Inter()}
  };
  return new oatpp::Type(CLASS_ID, info);
}

oatpp::Type* DateTime2::getType() {
  static Type* type = createType();
  return type;
}

oatpp::Type* DateTimeOffset::createType() {
  Type::Info info;
  info.interpretationMap = {
    {"oatpp::String", new Inter()}
  };
  return new oatpp::Type(CLASS_ID, info);
}

oatpp::Type* DateTimeOffset::getType() {
  static Type* type = createType();
  return type;
}

oatpp::Type* Time::createType() {
  Type::Info info;
  info.interpretationMap = {
    {"oatpp::String", new Inter()}
  };
  return new oatpp::Type(CLASS_ID, info);
}

oatpp::Type* Time::getType() {
  static Type* type = createType();
  return type;
}

oatpp::Type* Date::createType() {
  Type::Info info;
  info.interpretationMap = {
    {"oatpp::String", new Inter()}
  };
  return new oatpp::Type(CLASS_ID, info);
}

oatpp::Type* Date::getType() {
  static Type* type = createType();
  return type;
}

}

// Simplified implementations for other types (DateTimeOffset, Time, Date)
// These would be fully implemented in a production system

DateTimeOffsetObject::DateTimeOffsetObject(const DateTime2Object& dateTime, const std::chrono::minutes& offsetMinutes)
  : m_dateTime(dateTime), m_offsetMinutes(offsetMinutes) {}

DateTimeOffsetObject::DateTimeOffsetObject(const std::string& isoString, v_int32 fractionalPrecision)
  : m_dateTime(isoString, fractionalPrecision), m_offsetMinutes(0) {
  // Parse timezone offset from ISO string - simplified implementation
}

DateTime2Object DateTimeOffsetObject::getDateTime() const {
  return m_dateTime;
}

std::chrono::minutes DateTimeOffsetObject::getOffset() const {
  return m_offsetMinutes;
}

DateTime2Object DateTimeOffsetObject::toUTC() const {
  return m_dateTime.subtract(std::chrono::duration_cast<std::chrono::nanoseconds>(m_offsetMinutes));
}

std::string DateTimeOffsetObject::toISOString() const {
  auto utcString = m_dateTime.toISOString();
  // Remove Z and add offset
  if (!utcString.empty() && utcString.back() == 'Z') {
    utcString.pop_back();
  }
  
  auto offsetHours = m_offsetMinutes.count() / 60;
  auto offsetMins = m_offsetMinutes.count() % 60;
  
  std::ostringstream oss;
  oss << utcString << (offsetHours >= 0 ? "+" : "") 
      << std::setfill('0') << std::setw(2) << offsetHours
      << ":" << std::setfill('0') << std::setw(2) << std::abs(offsetMins);
  
  return oss.str();
}

std::string DateTimeOffsetObject::toSqlServerString() const {
  return m_dateTime.toSqlServerString() + " " + 
         (m_offsetMinutes.count() >= 0 ? "+" : "-") +
         std::to_string(std::abs(m_offsetMinutes.count() / 60)) + ":" +
         std::to_string(std::abs(m_offsetMinutes.count() % 60));
}

bool DateTimeOffsetObject::operator==(const DateTimeOffsetObject& other) const {
  return toUTC().getTimePoint() == other.toUTC().getTimePoint();
}

bool DateTimeOffsetObject::operator!=(const DateTimeOffsetObject& other) const {
  return !(*this == other);
}

bool DateTimeOffsetObject::operator<(const DateTimeOffsetObject& other) const {
  return toUTC().getTimePoint() < other.toUTC().getTimePoint();
}

bool DateTimeOffsetObject::operator<=(const DateTimeOffsetObject& other) const {
  return toUTC().getTimePoint() <= other.toUTC().getTimePoint();
}

bool DateTimeOffsetObject::operator>(const DateTimeOffsetObject& other) const {
  return toUTC().getTimePoint() > other.toUTC().getTimePoint();
}

bool DateTimeOffsetObject::operator>=(const DateTimeOffsetObject& other) const {
  return toUTC().getTimePoint() >= other.toUTC().getTimePoint();
}

// TimeObject simplified implementation
TimeObject::TimeObject(v_int32 hour, v_int32 minute, v_int32 second, v_int32 nanosecond, v_int32 fractionalPrecision)
  : m_fractionalPrecision(fractionalPrecision) {
  m_timeOfDay = std::chrono::hours(hour) + std::chrono::minutes(minute) + 
                std::chrono::seconds(second) + std::chrono::nanoseconds(nanosecond);
}

TimeObject::TimeObject(const std::string& timeString, v_int32 fractionalPrecision)
  : m_fractionalPrecision(fractionalPrecision) {
  // Parse HH:MM:SS.nnnnnnn format - simplified
  m_timeOfDay = std::chrono::hours(0); // Placeholder
}

std::string TimeObject::toString() const {
  auto hours = std::chrono::duration_cast<std::chrono::hours>(m_timeOfDay);
  auto minutes = std::chrono::duration_cast<std::chrono::minutes>(m_timeOfDay - hours);
  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(m_timeOfDay - hours - minutes);
  auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(m_timeOfDay - hours - minutes - seconds);
  
  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(2) << hours.count() << ":"
      << std::setfill('0') << std::setw(2) << minutes.count() << ":"
      << std::setfill('0') << std::setw(2) << seconds.count();
  
  if (m_fractionalPrecision > 0) {
    v_int64 fracValue = nanoseconds.count();
    v_int64 divisor = 1;
    for (int i = 0; i < (9 - m_fractionalPrecision); ++i) {
      divisor *= 10;
    }
    fracValue /= divisor;
    oss << "." << std::setfill('0') << std::setw(m_fractionalPrecision) << fracValue;
  }
  
  return oss.str();
}

// DateObject simplified implementation
DateObject::DateObject(v_int32 year, v_int32 month, v_int32 day)
  : m_year(year), m_month(month), m_day(day) {
  validateDate(year, month, day);
}

DateObject::DateObject(const std::string& dateString) {
  // Parse YYYY-MM-DD format - simplified
  std::regex date_regex(R"((\d{4})-(\d{2})-(\d{2}))");
  std::smatch matches;
  
  if (std::regex_match(dateString, matches, date_regex)) {
    m_year = std::stoi(matches[1]);
    m_month = std::stoi(matches[2]);
    m_day = std::stoi(matches[3]);
    validateDate(m_year, m_month, m_day);
  } else {
    throw std::invalid_argument("Invalid date format: " + dateString);
  }
}

void DateObject::validateDate(v_int32 year, v_int32 month, v_int32 day) {
  if (year < 1 || year > 9999) {
    throw std::invalid_argument("Year must be between 1 and 9999");
  }
  if (month < 1 || month > 12) {
    throw std::invalid_argument("Month must be between 1 and 12");
  }
  if (day < 1 || day > getDaysInMonth(year, month)) {
    throw std::invalid_argument("Invalid day for given month and year");
  }
}

bool DateObject::isLeapYear(v_int32 year) const {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

v_int32 DateObject::getDaysInMonth(v_int32 year, v_int32 month) const {
  static const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (month == 2 && isLeapYear(year)) {
    return 29;
  }
  return daysInMonth[month - 1];
}

std::string DateObject::toString() const {
  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(4) << m_year << "-"
      << std::setfill('0') << std::setw(2) << m_month << "-"
      << std::setfill('0') << std::setw(2) << m_day;
  return oss.str();
}

v_int32 DateObject::getYear() const { return m_year; }
v_int32 DateObject::getMonth() const { return m_month; }
v_int32 DateObject::getDay() const { return m_day; }

bool DateObject::operator==(const DateObject& other) const {
  return m_year == other.m_year && m_month == other.m_month && m_day == other.m_day;
}

bool DateObject::operator!=(const DateObject& other) const {
  return !(*this == other);
}

bool DateObject::operator<(const DateObject& other) const {
  if (m_year != other.m_year) return m_year < other.m_year;
  if (m_month != other.m_month) return m_month < other.m_month;
  return m_day < other.m_day;
}

bool DateObject::operator<=(const DateObject& other) const {
  return *this < other || *this == other;
}

bool DateObject::operator>(const DateObject& other) const {
  return !(*this <= other);
}

bool DateObject::operator>=(const DateObject& other) const {
  return !(*this < other);
}

}}}}