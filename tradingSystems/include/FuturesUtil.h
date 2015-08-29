#ifndef _FUTURES_UTIL_
#define _FUTURES_UTIL_

#include <string>
#include "FuturesDataStructures.h"
#include "../CTPLib/ThostTraderApi/ThostFtdcUserApiStruct.h"
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

using namespace std;

#define FuturesDbName "Futures";


bool floatNumberLess(double first, double second);
bool floatNumberEqual(double first, double second);
bool floatNumberLessEqual(double first, double second);

template<typename T> string toString(T val){
	stringstream ss;
	ss << val;
	return ss.str();
}

struct FuturesUtil{
	/************************************************************************/
	/* Translating the CTP data structures                                                                     */
	/************************************************************************/
	static string futuresOrderType(TThostFtdcDirectionType direction, TThostFtdcCombOffsetFlagType comboFlag);

	static string futuresRspInfoToString(CThostFtdcRspInfoField *p);

	static string futuresTickToString(CThostFtdcDepthMarketDataField * p);

	static string futuresOrderFieldToString(CThostFtdcOrderField * p);

	static string futuresInputOrderFieldToString(CThostFtdcInputOrderField * p);

	static string futuresInputOrderActionFieldToString(CThostFtdcInputOrderActionField * p);

	static string futuresOrderActionFieldToString(CThostFtdcOrderActionField * p);

	static string futuresTradeFieldToString(CThostFtdcTradeField * p);

	static string futuresPositionToString(const CThostFtdcInvestorPositionField& position);

	static string orderStatusToString(TThostFtdcOrderStatusType status);

	static string orderSubmissionStatusToString(TThostFtdcOrderSubmitStatusType status);

	static string orderActionStatusToString(TThostFtdcOrderActionStatusType status);

	static int approximateMinuteDiff(int minute1, int minute2);

	static string getTimeSecondsAfter(const string& timeStr, int seconds);

	static string getIthComponentOfTime(const string& timeStr, int i);

	static FuturesConfigInfo LoadConfigureFile(const string &filename);

	static vector<string> &split(const string &s, char delim, vector<string> &elems);

	static vector<string> split(const string &s, char delim);

	static string getCurrentDateString();

	static string getCurrentTimeString();

	static double safeDivide(double a, double b) {
		return b == 0 ? 0 : a / b;
	}

};


#endif // !_FUTURES_UTIL_
