#ifndef _MYSQL_CONNECTION_H_
#define _MYSQL_CONNECTION_H_

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/sqlstring.h>
#include <cppconn/prepared_statement.h>

using namespace std;
using namespace sql;
using namespace mysql;

#define MYSQL_END_POINT			"tcp://localhost:3306"
#define MYSQL_USER_NAME			"yang.ye"
#define MYSQL_PASSWORD			"yang.ye"


class MySqlConnector {
private:
  mysql::MySQL_Driver *driver;
  Connection *con;
  Statement *state;
  void reconnectIfExpired();

public:
  MySqlConnector();

  ~MySqlConnector();

  ResultSet* queryWithoutRetry(const string& qry);

  bool executeWithoutRetry(const string& qry);

  bool excuteUpdateWithoutRetry(const string& qry);

  ResultSet* query(const string& qry);

  bool execute(const string& qry);

  int executeUpdate(const string& qry);

};

extern MySqlConnector mysqlConnector;

#endif
