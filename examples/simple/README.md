# Oat++ SQL Server Simple Example

This example demonstrates how to use the oatpp-sqlserver adapter to connect to and interact with SQL Server databases.

## Features Demonstrated

- ✅ Cross-platform SQL Server connectivity (Windows/Linux)
- ✅ Basic database operations (CREATE, INSERT, SELECT, UPDATE, DROP)
- ✅ Transaction support
- ✅ Error handling
- ✅ Connection pooling support
- ✅ ODBC driver integration

## Prerequisites

### Windows
1. Install Microsoft ODBC Driver for SQL Server (version 17 or 18)
2. SQL Server instance (Express, Developer, or full version)

### Linux
1. Install unixODBC development libraries:
   ```bash
   sudo apt-get install unixodbc-dev
   ```
2. Install Microsoft ODBC Driver for SQL Server:
   ```bash
   curl https://packages.microsoft.com/keys/microsoft.asc | apt-key add -
   curl https://packages.microsoft.com/config/ubuntu/$(lsb_release -rs)/prod.list > /etc/apt/sources.list.d/mssql-release.list
   sudo apt-get update
   sudo ACCEPT_EULA=Y apt-get install msodbcsql18
   ```

## Connection String Examples

The example includes several connection string templates:

### 1. SQL Server Express with Windows Authentication
```cpp
Driver={ODBC Driver 18 for SQL Server};Server=ServerName\SQLEXPRESS;Trusted_Connection=Yes;Encrypt=No;TrustServerCertificate=Yes;
```

### 2. SQL Server with SQL Authentication
```cpp
Driver={ODBC Driver 18 for SQL Server};Server=localhost;Database=testdb;UID=sa;PWD=YourPassword123;Encrypt=No;TrustServerCertificate=Yes;
```

### 3. Azure SQL Database
```cpp
Driver={ODBC Driver 18 for SQL Server};Server=yourserver.database.windows.net;Database=yourdatabase;UID=yourusername;PWD=yourpassword;Encrypt=Yes;
```

## Building

1. Build the main oatpp-sqlserver library:
   ```bash
   mkdir build && cd build
   cmake .. -DOATPP_MODULES_LOCATION=SUBMODULE
   make oatpp-sqlserver
   ```

2. Build the example:
   ```bash
   make simple-sqlserver
   ```

## Running

Update the connection string in `main.cpp` to match your SQL Server configuration, then:

```bash
./simple-sqlserver
```

## Expected Output

When connected to a SQL Server instance, you should see:

```
=== Oat++ SQL Server Example ===
Connection String: [your connection string]

1. Testing basic connectivity...
✓ Connected to SQL Server successfully.

2. Setting up SQL executor...
✓ SQL executor created.

3. Creating test table...
✓ Dropped existing TestUsers table (if any).
✓ TestUsers table created successfully.

4. Inserting sample data...
✓ Inserted user: John Doe
✓ Inserted user: Jane Smith
✓ Inserted user: Bob Johnson (inactive)

5. Querying all users...
✓ Query successful. Users in table:
  (Note: Advanced result mapping requires full ORM setup)
  - Query executed without errors
  - Position: 0
  - Has more data: No

6. Updating user data...
✓ User updated successfully.

7. Testing transaction support...
✓ Transaction started.
✓ Transaction rolled back successfully.

8. Cleaning up...
✓ TestUsers table dropped successfully.

=== Example completed successfully! ===

This example demonstrates:
  • SQL Server connection via ODBC
  • Table creation and management
  • Data insertion, querying, and updates
  • Transaction support
  • Error handling
  • Cross-platform compatibility (Windows/Linux)
```

## Notes

- This example uses direct SQL execution rather than full ORM features for simplicity
- For production applications, consider using the full ORM features with DTOs and query builders
- The example includes comprehensive error handling and cleanup
- All SQL operations are executed within proper transactions where applicable

## Troubleshooting

### Common Connection Issues

1. **Driver not found**: Install the Microsoft ODBC Driver for SQL Server
2. **Connection timeout**: Check server name, port, and network connectivity
3. **Authentication failed**: Verify credentials and SQL Server authentication mode
4. **SSL/TLS errors**: Adjust `Encrypt` and `TrustServerCertificate` settings in connection string

### ODBC Driver Versions

- Use "ODBC Driver 18 for SQL Server" for newest features
- Use "ODBC Driver 17 for SQL Server" for broader compatibility
- Use "SQL Server" driver only as a last resort (deprecated)