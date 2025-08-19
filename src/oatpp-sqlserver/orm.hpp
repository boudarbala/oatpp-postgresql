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
 * ```
 *
 * Advanced Query Building:
 * ```cpp
 * #include "QueryBuilder.hpp"
 * #include "AdvancedDbClient.hpp"
 * ```
 *
 * Enhanced Type System:
 * - High-precision Decimal type for DECIMAL/NUMERIC columns
 * - DateTime2 type with fractional seconds precision (0-7)
 * - DateTimeOffset type with timezone information
 * - Time type for time-of-day values
 * - Date type for date-only values
 * - UUID type for UNIQUEIDENTIFIER columns
 *
 * Advanced Features:
 * - Sophisticated query builder with CTEs, window functions
 * - Bulk operations for high-performance data loading
 * - Stored procedure execution with typed parameters
 * - Performance monitoring and query metrics
 * - Connection health monitoring
 * - Pagination support with OFFSET/FETCH NEXT
 * - MERGE statement support for upsert operations
 * - Table-valued parameters (future)
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
#include "QueryBuilder.hpp"
#include "AdvancedDbClient.hpp"

#include "oatpp/orm/SchemaMigration.hpp"
#include "oatpp/orm/DbClient.hpp"
#include "oatpp/macro/codegen.hpp"

#endif // oatpp_sqlserver_orm_hpp
