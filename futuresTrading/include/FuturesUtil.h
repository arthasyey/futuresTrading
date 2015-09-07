#ifndef _FUTURES_UTIL_
#define _FUTURES_UTIL_

#include <FuturesDataStructures.h>
#include <logging.h>

#include <string>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

using namespace std;

#define FuturesDbName "futures";


#define LOG_BOOST BOOST_LOG_SEV(lg, info) << "--->>> " << __PRETTY_FUNCTION__

bool floatNumberLess(double first, double second);
bool floatNumberEqual(double first, double second);
bool floatNumberLessEqual(double first, double second);

string concatenate(const vector<string>& v, const char* delims, bool tailDelim = true);

vector<string>& split(const string &s, char delim, vector<string> &elems);

vector<string> split(const string &s, char delim);

template<typename T> string toString(T val){
  stringstream ss;
  ss << val;
  return ss.str();
}

struct FuturesUtil {
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

  static string rejectInfoToString(RejectInfo * p);

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

  static string getContractTypeFromSymbol(const string& symbol);

  static string getExchangeFromSymbol(const string& symbol);

  static map<string, FuturesContractInfo> initContractInfos();

  FuturesUtil();
};


#endif // !_FUTURES_UTIL_
