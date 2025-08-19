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
 * SQL Server type definitions for ODBC.
 */

#ifndef oatpp_sqlserver_mapping_Oid_hpp
#define oatpp_sqlserver_mapping_Oid_hpp

/***************************************************************************
 * SQL Server/ODBC type definitions
 ***************************************************************************/

// Ensure Windows and SAL annotations are defined before ODBC headers
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <sal.h>
#include <sql.h>
#include <sqlext.h>

// Handle Windows BOOL macro conflict with oatpp
#ifdef BOOL
#undef BOOL
#endif

// SQL Server data type mappings
#define SQL_TYPE_VARCHAR        SQL_VARCHAR
#define SQL_TYPE_CHAR           SQL_CHAR
#define SQL_TYPE_NVARCHAR       SQL_WVARCHAR
#define SQL_TYPE_NCHAR          SQL_WCHAR
#define SQL_TYPE_INT            SQL_INTEGER
#define SQL_TYPE_SMALLINT       SQL_SMALLINT
#define SQL_TYPE_BIGINT         SQL_BIGINT
#define SQL_TYPE_REAL           SQL_REAL
#define SQL_TYPE_FLOAT          SQL_FLOAT
#define SQL_TYPE_DOUBLE         SQL_DOUBLE
#define SQL_TYPE_DECIMAL        SQL_DECIMAL
#define SQL_TYPE_NUMERIC        SQL_NUMERIC
#define SQL_TYPE_BIT            SQL_BIT
#define SQL_TYPE_TINYINT        SQL_TINYINT
#define SQL_TYPE_DATETIME       SQL_TYPE_TIMESTAMP
#define SQL_TYPE_SMALLDATETIME  SQL_TYPE_TIMESTAMP
#define SQL_TYPE_DATETIME2      SQL_TYPE_TIMESTAMP
#define SQL_TYPE_SQLDATE        SQL_TYPE_DATE
#define SQL_TYPE_SQLTIME        SQL_TYPE_TIME
#define SQL_TYPE_UNIQUEIDENTIFIER SQL_GUID
#define SQL_TYPE_BINARY         SQL_BINARY
#define SQL_TYPE_VARBINARY      SQL_VARBINARY
#define SQL_TYPE_IMAGE          SQL_LONGVARBINARY
#define SQL_TYPE_TEXT           SQL_LONGVARCHAR
#define SQL_TYPE_NTEXT          SQL_WLONGVARCHAR

// Use SQL types instead of PostgreSQL OIDs
typedef SQLSMALLINT SqlOid;
typedef SQLSMALLINT Oid; // Compatibility alias
#define InvalidSqlOid           SQL_UNKNOWN_TYPE
#define InvalidOid              InvalidSqlOid

// SQL Server OID mappings using SQL data types
// Use distinct values to avoid duplicate case labels
#define TEXTOID                 SQL_LONGVARCHAR      // SQL Server TEXT type
#define TEXTARRAYOID            (SQL_LONGVARCHAR+1000)  // Array handling will be different in SQL Server
#define VARCHAROID              SQL_VARCHAR
#define VARCHARARRAYOID         (SQL_VARCHAR+1000)
#define CHAROID                 SQL_CHAR
#define BPCHAROID               (SQL_CHAR+100)    // PostgreSQL BPCHAR -> distinct from CHAR

#define INT2OID                 SQL_SMALLINT
#define INT2ARRAYOID            (SQL_SMALLINT+1000)
#define INT4OID                 SQL_INTEGER  
#define INT4ARRAYOID            (SQL_INTEGER+1000)
#define INT8OID                 SQL_BIGINT
#define INT8ARRAYOID            (SQL_BIGINT+1000)

#define FLOAT4OID               SQL_REAL
#define FLOAT4ARRAYOID          (SQL_REAL+1000)
#define FLOAT8OID               SQL_DOUBLE
#define FLOAT8ARRAYOID          (SQL_DOUBLE+1000)

#define BOOLOID                 SQL_BIT
#define BOOLARRAYOID            (SQL_BIT+1000)

#define TIMESTAMPOID            SQL_TYPE_TIMESTAMP
#define TIMESTAMPARRAYOID       (SQL_TYPE_TIMESTAMP+1000)

#define UUIDOID                 SQL_GUID
#define UUIDARRAYOID            (SQL_GUID+1000)

#endif // oatpp_sqlserver_mapping_Oid_hpp