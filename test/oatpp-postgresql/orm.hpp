/***************************************************************************
 * Compatibility shim for tests expecting oatpp-postgresql headers.
 * For the sqlserver port, forward to oatpp-sqlserver and alias namespace.
 ***************************************************************************/
#pragma once

#include <oatpp-sqlserver/orm.hpp>

namespace oatpp {
  namespace postgresql = sqlserver;
}
