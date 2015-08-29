#ifndef _UTILS_H_
#define _UTILS_H_
#include <string>
#include <vector>
#include <iostream>
#include <iomanip> 
#include <map>
#include <boost/algorithm/string.hpp>    
#include "logging.h"
#include <boost/format.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/thread.hpp>

using namespace std;
enum ExchangeType;
enum Side;


template<typename T> T stringToType(const string& str) {
	stringstream ss;
	ss << str;
	T t;
	ss >> t;
	return t;
}

class Utils {
public:	

	static string replaceString(const string& str, const string &src, const string &dest);

	static string getNextDay(const string& day);

	static string entrustTimeFormat(const string& entrustTime);

	static string concatenate(const vector<string>& v, const char* delims, bool tailDelim = true);

	static vector<string> &split(const string &s, char delim, vector<string> &elems);

	static vector<string> split(const string &s, char delim);

	static string getFormattedTime(const char* format = "%Y%m%d");

	static void flatLogging(const string& str);

	static string roundToHundredth(const string& str);

	static string roundStrToInteger(const string& str);

	static int roundDown(int numberToRound, int multiple); 

	static int roundUp(int numToRound, int multiple);	

	static bool isCreditAccount(const string& account);	

	static string exchangeOrWindCode2Exchange(const string& code);

	static string exchangeOrWindCode2Wind(const string& code);

	static string exchangeCode2Wind(const string& code);

	static string windCode2Exchange(const string& code);

	static ExchangeType getExchangeType(const string& code);

	static double roundToHundredth(double val);		

	static string doubleTostring(double data);
	
	static double stringTodouble(const string &str, int precision = 2) {
		return atof(str.c_str());
	}

	static int stringToInt(const string& str) {
		return atoi(str.c_str());
	}

	static string precisionOut(double v, int precision);

	static double roundDouble(double v, unsigned int precision);

	static int roundToInt(double v);

	static string getDate(const char *date);
	static string getTime(const char *time);


	static tm getCurrentTime();

	static string getCurrentDateString();
	static string getCurrentTimeString();

	static struct timespec getTimeNSecondsFromNow(int n);

	static double getAvgTradeCost(const string& symbol, int nShares, double price, Side side);
	
	static string getFirstLetter(const char *strChs);


	static void boostCVSafeWait(boost::condition_variable& cv, boost::mutex::scoped_lock &lk, int seconds);

	static double safeDivide(double a, double b) {
		if (b == 0)
			return 0;
		else
			return a / b;
	}

};


#endif