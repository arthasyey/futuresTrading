#include "../include/mysqlConnector.h"
#include "../include/logging.h"

MySqlConnector::MySqlConnector() :driver(sql::mysql::get_mysql_driver_instance()), con(NULL), state(NULL)
{
  try {
    con = driver->connect("tcp://localhost:3306", "yeyang", "yeyang");
    state = con->createStatement();
  }
  catch (exception& e) {

  }
}

ResultSet* MySqlConnector::queryWithoutRetry(const string& qry){
  try {
    return state->executeQuery(qry);
  }
  catch (SQLException &e){
    flatLogging("Exception: " + string(e.what()) + e.getSQLState() + " when query: " + qry);
    return NULL;
  }
}

bool MySqlConnector::executeWithoutRetry(const string& qry) {
  try {
    return state->execute(qry);
  }
  catch (SQLException &e){
    flatLogging("Exception: " + string(e.what()) + e.getSQLState() + " when execute: " + qry);
    return false;
  }
}

bool MySqlConnector::excuteUpdateWithoutRetry(const string& qry) {
  try {
    return state->executeUpdate(qry);
  }
  catch (SQLException &e){
    flatLogging("Exception: " + string(e.what()) + e.getSQLState() + " when execute: " + qry);
    return false;
  }
}


ResultSet* MySqlConnector::query(const string& qry){
  try
  {
    flatLogging("Query: " + qry);
    return state->executeQuery(qry);
  }
  catch (SQLException& e)
  {
    flatLogging("Exception: " + string(e.what()) + e.getSQLState() + " when execute: " + qry + " , Retry!");
    return queryWithoutRetry(qry);
  }
}

bool MySqlConnector::execute(const string& qry) {
  try
  {
    flatLogging("Query: " + qry);
    return state->execute(qry);
  }
  catch (SQLException& e)
  {
    flatLogging("Exception: " + string(e.what()) + e.getSQLState() + " when execute: " + qry + " , Retry!");
    return executeWithoutRetry(qry);
  }
  catch (exception& e)
  {
    flatLogging("Exception: " + string(e.what()) + " when execute: " + qry + " , Retry!");
    return executeWithoutRetry(qry);
  }
}

int MySqlConnector::executeUpdate(const string& qry) {
  try {
    flatLogging("Query: " + qry);
    return state->executeUpdate(qry);
  }
  catch (SQLException& e)
  {
    flatLogging("Exception: " + e.getSQLState() + " when execute: " + qry + " , Retry!");
    return excuteUpdateWithoutRetry(qry);
  }
}

MySqlConnector::~MySqlConnector() {}
