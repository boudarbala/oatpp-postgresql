#include "oatpp/Environment.hpp"
#include "oatpp-sqlserver/ConnectionProvider.hpp"

#include <iostream>

int main() {
  oatpp::Environment::init();

  const auto connectionString = oatpp::String(R"(Driver={ODBC Driver 18 for SQL Server};Server=BOUDARBALA\SQLEXPRESS;Trusted_Connection=Yes;Encrypt=No;TrustServerCertificate=Yes;)");

  try {
    auto provider = std::make_shared<oatpp::sqlserver::ConnectionProvider>(connectionString);

    auto handle = provider->get();
    if(handle) {
      std::cout << "Connected to SQL Server successfully." << std::endl;
    }

    // Release connection by letting handle go out of scope

  } catch (const std::exception& e) {
    std::cerr << "Connection failed: " << e.what() << std::endl;
    oatpp::Environment::destroy();
    return 1;
  }

  oatpp::Environment::destroy();
  return 0;
}
