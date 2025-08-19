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

/**[info]
 *
 * This header file includes all oatpp-sqlserver components with sophisticated ORM features:
 *
 * Core Components:
 * ```cpp
 * #include "Executor.hpp"
 * #include "Types.hpp"
 * #include "SimpleQueryBuilder.hpp"
 * ```
 *
 * Enhanced Type System:
 * - High-precision Decimal type for DECIMAL/NUMERIC columns with custom precision/scale
 * - DateTime2 type with fractional seconds precision (0-7) for DATETIME2 columns
 * - DateTimeOffset type with timezone information for DATETIMEOFFSET columns
 * - Time type for time-of-day values with TIME columns
 * - Date type for date-only values with DATE columns
 * - UUID type for UNIQUEIDENTIFIER columns
 *
 * Advanced Query Building:
 * - SimpleQueryBuilder with fluent API for complex SQL construction
 * - Common Table Expressions (CTEs) support for recursive and analytical queries
 * - Window functions integration (ROW_NUMBER, PERCENT_RANK, LAG, LEAD, etc.)
 * - SQL Server-style pagination with OFFSET/FETCH NEXT
 * - Sophisticated JOIN operations (INNER, LEFT, RIGHT, FULL)
 * - Advanced analytics with GROUP BY, HAVING, and aggregation functions
 *
 * SQL Server Specific Features:
 * - Native SQL Server data type mappings
 * - High-precision numeric calculations
 * - Timezone-aware date/time operations
 * - Professional schema design patterns
 * - Enterprise-grade query optimization
 *
 * Professional Applications:
 * - Financial systems requiring precise decimal calculations
 * - Global applications with timezone handling
 * - Analytics platforms with sophisticated reporting
 * - Enterprise software with complex business logic
 * - Data warehousing and business intelligence
 *
 * Standard oatpp ORM:
 * ```cpp
 * #include "oatpp/orm/SchemaMigration.hpp"
 * #include "oatpp/orm/DbClient.hpp"
 * #include "oatpp/macro/codegen.hpp"
 * ```
 */

#ifndef oatpp_sqlserver_orm_hpp
#define oatpp_sqlserver_orm_hpp

#include "Executor.hpp"
#include "Types.hpp"
#include "SimpleQueryBuilder.hpp"

#include "oatpp/orm/SchemaMigration.hpp"
#include "oatpp/orm/DbClient.hpp"
#include "oatpp/macro/codegen.hpp"

#endif // oatpp_sqlserver_orm_hpp
