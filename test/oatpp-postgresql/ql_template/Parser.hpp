/***************************************************************************
 * Compatibility shim for tests expecting oatpp-postgresql ql_template headers.
 * For the sqlserver port, forward to oatpp-sqlserver.
 ***************************************************************************/
#pragma once

#include <oatpp-sqlserver/ql_template/Parser.hpp>

namespace oatpp {
  namespace postgresql {
    namespace ql_template {
      using Parser = ::oatpp::sqlserver::ql_template::Parser;
    }
  }
}
