#include <mysqlConnector.h>
#include "../include/utils.h"

#pragma comment(lib,"libmysql.lib")
#pragma comment(lib,"mysqlcppconn.lib")

MySqlConnector::MySqlConnector() :driver(sql::mysql::get_mysql_driver_instance()), con(NULL), state(NULL)
{
	reconnect();
}

MySqlConnector::~MySqlConnector()
{
	try {
		if (state)
		{			
			delete state;
		} 
		if (con)
		{
			if (!con->isClosed()) con->close();
			delete con;
		}
	}
	catch (exception &e) {
		flatLogging("Close Exception: " + string(e.what()));
	}
}

ResultSet* MySqlConnector::queryWithoutRetry(const string& qry){
	try {
		reconnect();
		return state->executeQuery(qry);
	}
	catch (SQLException &e){
		flatLogging("Retry Exception: " + string(e.what()) + e.getSQLState() + " when query: " + qry);
		return NULL;
	}
}

void MySqlConnector::reconnect() {
	try {
		if (state)
		{
			delete state;
		}
		if (con)
		{
			if (!con->isClosed()) con->close();
			delete con;
		}
		con = driver->connect(MYSQL_END_POINT, MYSQL_USER_NAME, MYSQL_PASSWORD);
		state = con->createStatement();
		state->execute("SET NAMES gb2312");
	}
	catch (exception &e) {
		flatLogging("Reconnect Exception: " + string(e.what()));
	}
}

bool MySqlConnector::executeWithoutRetry(const string& qry) {
	try {
		reconnect();
		return state->execute(qry);
	}
	catch (SQLException &e){
		flatLogging("Retry Exception: " + string(e.what()) + e.getSQLState() + " when execute: " + qry);
		return false;
	}
}

bool MySqlConnector::excuteUpdateWithoutRetry(const string& qry) {
	try {
		reconnect();
		return state->executeUpdate(qry);
	}
	catch (SQLException &e){
		flatLogging("Retry Exception: " + string(e.what()) + e.getSQLState() + " when execute: " + qry);
		return false;
	}
}


ResultSet* MySqlConnector::query(const string& qry){
	try
	{
		if (!state)
		{
			reconnect();
		}
		flatLogging("Query: " + qry);
		//reconnect();
		return state->executeQuery(qry);
	}
	catch (SQLException& e)
	{
		flatLogging("Exception: " + string(e.what()) + e.getSQLState() + " when execute: " + qry + " , Retry!");
		//return queryWithoutRetry(qry);
	}
}

bool MySqlConnector::execute(const string& qry) {
	try
	{
		if (!state)
		{
			reconnect();
		}
		flatLogging("Query: " + qry);
		//reconnect();
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
		if (!state)
		{
			reconnect();
		}
		flatLogging("Query: " + qry);
		//reconnect();
		return state->executeUpdate(qry);
	}
	catch (SQLException& e)
	{
		flatLogging("Exception: " + e.getSQLState() + " when execute: " + qry + " , Retry!");
		return excuteUpdateWithoutRetry(qry);
	}
}
