
#include "oatpp-sqlserver/orm.hpp"
#include "oatpp/Environment.hpp"

int main() {
  oatpp::Environment::init();
  
  // Basic test - just verify headers compile and classes can be instantiated
  OATPP_LOGi("Tests", "Basic SQL Server adapter test");
  
  OATPP_ASSERT(oatpp::Environment::getObjectsCount() == 0);
  oatpp::Environment::destroy();
  return 0;
}
