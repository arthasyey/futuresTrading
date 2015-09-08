#include "../include/FuturesUtil.h"
#include "../include/logging.h"
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <functional>
#include <json.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

using namespace boost::filesystem;

map<string, FuturesContractInfo> contractInfos = FuturesUtil::initContractInfos();

bool floatNumberLess(double first, double second) {
  return first < second - numeric_limits<double>::epsilon();
}

bool floatNumberEqual(double first, double second) {
  return abs(first - second) <= numeric_limits<double>::epsilon();
}

bool floatNumberLessEqual(double first, double second) {
  return first - second <= numeric_limits<double>::epsilon();
}

string concatenate(const vector<string>& v, const char* delims, bool tailDelim) {
  stringstream ss;
  copy(v.begin(), v.end(), ostream_iterator<string>(ss, delims));
  string result = ss.str();
  if (!tailDelim)
    {
      result = result.substr(0, result.find_last_of(delims));
    }
  return result;
}

vector<string>& split(const string &s, char delim, vector<string> &elems) {
  stringstream ss(s);
  string item;
  while (getline(ss, item, delim)) {
      elems.push_back(item);
  }
  return elems;
}

vector<string> split(const string &s, char delim) {
  vector<string> elems;
  split(s, delim, elems);
  return elems;
}

string FuturesUtil::getCurrentDateString() {
  stringstream ss;
#ifdef __WINDOWS__
  SYSTEMTIME sTime;
  GetLocalTime(&sTime);
  ss << sTime.wYear;
  if (sTime.wMonth < 10)
    {
      ss << '0';
    }
  ss << sTime.wMonth;
  if (sTime.wDay < 10)
    {
      ss << '0';
    }
  ss << sTime.wDay;
#else
#include<ctime>
  time_t t = time(0);   // get time now
  struct tm * now = localtime(&t);
  int year = now->tm_year + 1900;
  int month = now->tm_mon + 1;
  int day = now->tm_mday;
  ss << year;
  if (month < 10)
    ss << 0;
  ss << month;
  if (day < 10)
    ss << 0;
  ss << day;
#endif
  return ss.str();
}



string FuturesUtil::getCurrentTimeString() {
  stringstream ss;
#ifdef __WINDOWS__
  SYSTEMTIME sTime;
  GetLocalTime(&sTime);
  if (sTime.wHour < 10)
    {
      ss << '0';
    }
  ss << sTime.wHour << ':';
  if (sTime.wMinute < 10)
    {
      ss << '0';
    }
  ss << sTime.wMinute << ':';
  if (sTime.wSecond < 10)
    {
      ss << '0';
    }
  ss << sTime.wSecond;
#else
#endif
  return ss.str();
}

vector<std::string>& FuturesUtil::split(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
      elems.push_back(item);
  }
  return elems;
}

vector<std::string> FuturesUtil::split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  split(s, delim, elems);
  return elems;
}

string FuturesUtil::getTimeSecondsAfter(const string& timeStr, int secondsAdd) {
  vector<int> elems;
  stringstream ss(timeStr);
  string item;
  while (getline(ss, item, ':')) {
      elems.push_back(atoi(item.c_str()));
  }
  for (int i = elems.size(); i < 3; ++i)
    {
      elems.push_back(0);
    }

  int hour = elems[0];
  int minute = elems[1];
  int second = elems[2];

  second += secondsAdd;
  int minutesAdd = second / 60;
  second = second % 60;

  minute += minutesAdd;
  int hourAdd = minutesAdd / 60;
  minute = minute % 60;

  hour += hourAdd;

  stringstream ssNewTime;
  if (hour < 10)
    ssNewTime << "0";
  ssNewTime << hour << ":";

  if (minute < 10)
    ssNewTime << "0";
  ssNewTime << minute << ":";

  if (second < 10)
    ssNewTime << "0";
  ssNewTime << second;

  return ssNewTime.str();
}



int FuturesUtil::approximateMinuteDiff(int minute1, int minute2) {
  int diff = abs(minute1 - minute2);
  if (diff > 30)
    diff = 60 - diff;
  return diff;
}

string FuturesUtil::getIthComponentOfTime(const string& timeStr, int i) {
  vector<string> elems;
  stringstream ss(timeStr);
  string item;
  while (getline(ss, item, ':')) {
      elems.push_back(item);
  }
  return elems[i];
}


string FuturesUtil::futuresTickToString(CThostFtdcDepthMarketDataField * p) {
  stringstream ss;
  ss << "[CThostFtdcDepthMarketDataField ";
  if (p)
    {
      ss << "Date: " << p->TradingDay
	  << ", UpdateTime: " << p->UpdateTime
	  << ", UpdateMillisec: " << p->UpdateMillisec
	  << ", LastPrice: " << p->LastPrice
	  << ", BidPrice1: " << p->BidPrice1
	  << ", BidVolume1: " << p->BidVolume1
	  << ", AskPrice1: " << p->AskPrice1
	  << ", AskVolume1: " << p->AskVolume1 << "]";
    }
  return ss.str();
}

string FuturesUtil::futuresOrderType(TThostFtdcDirectionType direction, TThostFtdcCombOffsetFlagType comboFlag) {
  switch (direction)
  {
    case THOST_FTDC_D_Buy:  {
      if (comboFlag[0] == '0')
	return "OpenLong";
      else
	return "CloseShort";
    }
    case THOST_FTDC_D_Sell: {
      if (comboFlag[0] == '0')
	return "OpenShort";
      else
	return "CloseLong";
    }
    default:
      return "NA";
  }
}


string FuturesUtil::futuresRspInfoToString(CThostFtdcRspInfoField *p) {
  stringstream ss;
  ss << "[CThostFtdcRspInfoField ";
  if (p)
    {
      ss << " Response errorId: " << p->ErrorID
	  << ", errorMsg:" << p->ErrorMsg << "]";
    }
  return ss.str();
}


string FuturesUtil::futuresOrderFieldToString(CThostFtdcOrderField * p) {
  stringstream ss;
  ss << "[CThostFtdcOrderField: ";

  if (p)
    {
      ss << p->InstrumentID
	  << ", status:" << orderStatusToString(p->OrderStatus)
	  << ", submission status:" << orderSubmissionStatusToString(p->OrderSubmitStatus)
	  << ", type: " << futuresOrderType(p->Direction, p->CombHedgeFlag)
	  << ", total volume: " << p->VolumeTotalOriginal
	  << ", traded: " << p->VolumeTraded
	  << ", left: " << p->VolumeTotal
	  << ", limit price: " << p->LimitPrice
	  << ", FrontID: " << p->FrontID
	  << ", SessionID: " << p->SessionID
	  << ", OrderRef: " << p->OrderRef
	  << ", exchangeID: " << p->ExchangeID
	  << ", orderSysID: " << p->OrderSysID << "]";
    }
  return ss.str();
}


string FuturesUtil::futuresOrderActionFieldToString(CThostFtdcOrderActionField * p) {
  stringstream ss;
  ss << "[CThostFtdcOrderActionField: ";

  if (p)
    {
      if (p->ActionFlag == THOST_FTDC_AF_Delete)
	ss << "Delete";
      else
	ss << "Modify";

      ss << ", FrontID: " << p->FrontID
	  << ", SessionID: " << p->SessionID
	  << ", OrderRef: " << p->OrderRef
	  << ", exchangeID: " << p->ExchangeID
	  << ", orderSysID: " << p->OrderSysID << "]";
    }
  return ss.str();
}


string FuturesUtil::futuresInputOrderFieldToString(CThostFtdcInputOrderField * p) {
  stringstream ss;
  ss << "[CThostFtdcInputOrderField: ";

  if (p)
    {
      ss << p->InstrumentID
	  << ", type: " << futuresOrderType(p->Direction, p->CombHedgeFlag)
	  << ", total volume: " << p->VolumeTotalOriginal
	  << ", OrderRef: " << p->OrderRef << "]";
    }
  return ss.str();
}

string FuturesUtil::futuresInputOrderActionFieldToString(CThostFtdcInputOrderActionField * p) {
  stringstream ss;
  ss << "[CThostFtdcInputOrderActionField: ";

  if (p)
    {
      if (p->ActionFlag == THOST_FTDC_AF_Delete)
	ss << "Delete";
      else
	ss << "Modify";

      ss << ", FrontID: " << p->FrontID
	  << ", SessionID: " << p->SessionID
	  << ", OrderRef: " << p->OrderRef
	  << ", InstrumentID: " << p->InstrumentID
	  << ", exchangeID: " << p->ExchangeID
	  << ", orderSysID: " << p->OrderSysID << "]";
    }
  return ss.str();
}


string FuturesUtil::futuresTradeFieldToString(CThostFtdcTradeField * p) {
  stringstream ss;
  ss << "[CThostFtdcTradeField";
  if (p)
    {
      ss << p->InstrumentID
	  << ", type: " << p->Direction
	  << ", volume: " << p->Volume
	  << ", price: " << p->Price
	  << ", OrderRef: " << p->OrderRef
	  << ", exchangeID: " << p->ExchangeID
	  << ", orderSysID: " << p->OrderSysID << "]";
    }
  return ss.str();
}

string FuturesUtil::rejectInfoToString(RejectInfo * p) {
  stringstream ss;
  ss << "[RejectInfo";
  if (p)
    {
      ss << p->orderRef << ", reason: " << p->reason << "]";
    }
  return ss.str();
}

string FuturesUtil::futuresPositionToString(const CThostFtdcInvestorPositionField& position) {
  stringstream ss;
  ss << "InstrumentID: " << position.InstrumentID << "\n";
  ss << "BrokerID: " << position.BrokerID << "\n";
  ss << "InvestorID: " << position.InvestorID << "\n";
  ss << "PosiDirection: " << position.PosiDirection << "\n";
  ss << "YdPosition: " << position.YdPosition << "\n";
  ss << "Position: " << position.Position << "\n";
  ss << "LongFrozen: " << position.LongFrozen << "\n";
  ss << "ShortFrozen: " << position.ShortFrozen << "\n";
  ss << "LongFrozenAmount: " << position.LongFrozenAmount << "\n";
  ss << "ShortFrozenAmount: " << position.ShortFrozenAmount << "\n";
  ss << "OpenVolume: " << position.OpenVolume << "\n";
  ss << "CloseVolume: " << position.CloseVolume << "\n";
  ss << "OpenAmount: " << position.OpenAmount << "\n";
  ss << "CloseAmount: " << position.CloseAmount << "\n";
  ss << "PositionCost: " << position.PositionCost << "\n";
  ss << "PreMargin: " << position.PreMargin << "\n";
  ss << "UseMargin: " << position.UseMargin << "\n";
  ss << "FrozenMargin: " << position.FrozenMargin << "\n";
  ss << "FrozenCash: " << position.FrozenCash << "\n";
  ss << "FrozenCommission: " << position.FrozenCommission << "\n";
  ss << "CashIn: " << position.CashIn << "\n";
  ss << "Commission: " << position.Commission << "\n";
  ss << "CloseProfit: " << position.CloseProfit << "\n";
  ss << "PositionProfit: " << position.PositionProfit << "\n";
  ss << "PreSettlementPrice: " << position.PreSettlementPrice << "\n";
  ss << "SettlementPrice: " << position.SettlementPrice << "\n";
  ss << "TradingDay: " << position.TradingDay << "\n";
  ss << "SettlementID: " << position.SettlementID << "\n";
  ss << "OpenCost: " << position.OpenCost << "\n";
  ss << "ExchangeMargin: " << position.ExchangeMargin << "\n";
  ss << "CombPosition: " << position.CombPosition << "\n";
  ss << "CombLongFrozen: " << position.CombLongFrozen << "\n";
  ss << "CombShortFrozen: " << position.CombShortFrozen << "\n";
  ss << "CloseProfitByDate: " << position.CloseProfitByDate << "\n";
  ss << "CloseProfitByTrade: " << position.CloseProfitByTrade << "\n";
  ss << "TodayPosition: " << position.TodayPosition << "\n";
  ss << "MarginRateByMoney: " << position.MarginRateByMoney << "\n";
  ss << "MarginRateByVolume: " << position.MarginRateByVolume << "\n";
  return ss.str();
}


string FuturesUtil::orderStatusToString(TThostFtdcOrderStatusType status) {
  switch (status)
  {
    case THOST_FTDC_OST_AllTraded:
      return "AllTraded";

    case THOST_FTDC_OST_PartTradedQueueing:
      return "PartTradedQueueing";

    case THOST_FTDC_OST_PartTradedNotQueueing:
      return "PartTradedNotQueueing";

    case THOST_FTDC_OST_NoTradeQueueing:
      return "NoTradeQueueing";

    case THOST_FTDC_OST_NoTradeNotQueueing:
      return "NoTradeNotQueueing";

    case THOST_FTDC_OST_Canceled:
      return "Canceled";

    case THOST_FTDC_OST_Unknown:
      return "Unknown";

    case THOST_FTDC_OST_NotTouched:
      return "NotTouched";

    case THOST_FTDC_OST_Touched:
      return "Touched";

    default:
      return "NA";
  }
}


string FuturesUtil::orderSubmissionStatusToString(TThostFtdcOrderSubmitStatusType status) {
  switch (status)
  {
    case THOST_FTDC_OSS_InsertSubmitted:
      return "InsertSubmitted";
    case THOST_FTDC_OSS_CancelSubmitted:
      return "CancelSubmitted";
    case THOST_FTDC_OSS_ModifySubmitted:
      return  "ModifySubmitted";
    case THOST_FTDC_OSS_Accepted:
      return  "Accepted";
    case THOST_FTDC_OSS_InsertRejected:
      return  "InsertRejected";
    case THOST_FTDC_OSS_CancelRejected:
      return  "CancelRejected";
    case THOST_FTDC_OSS_ModifyRejected:
      return  "ModifyRejected";

    default:
      return "NA";
  }
}


string FuturesUtil::orderActionStatusToString(TThostFtdcOrderActionStatusType status) {
  switch (status)
  {
    case THOST_FTDC_OAS_Submitted:
      return "Submitted";
    case THOST_FTDC_OAS_Accepted:
      return  "Accepted";
    case THOST_FTDC_OAS_Rejected:
      return  "Rejected";

    default:
      return "NA";
  }
}

FuturesConfigInfo FuturesUtil::LoadConfigureFile(const string &filename) {
  FuturesConfigInfo configInfo;
  map<string, string> ConfigInfo;
  ifstream in;
  in.open(filename.c_str(), ios::in);
  if (in.is_open()) {
      string tmpStr;
      while (getline(in, tmpStr))
	{
	  int pos = tmpStr.find('=');
	  string strName = tmpStr.substr(0, pos);
	  string strValue = tmpStr.substr(pos + 1, tmpStr.length());
	  ConfigInfo[strName] = strValue;
	  cout << "config: " << tmpStr<< endl;
	}
      configInfo.TraderFrontAddr = boost::trim_copy(ConfigInfo["TraderFrontAddr"]);
      configInfo.MdFrontAddr = boost::trim_copy(ConfigInfo["MdFrontAddr"]);
      configInfo.BrokerId = boost::trim_copy(ConfigInfo["BrokerId"]);
      configInfo.UserId = boost::trim_copy(ConfigInfo["UserId"]);
      configInfo.Password = boost::trim_copy(ConfigInfo["Password"]);
      configInfo.MdBrokerId = boost::trim_copy(ConfigInfo["MdBrokerId"]);
      configInfo.MdUserId = boost::trim_copy(ConfigInfo["MdUserId"]);
      configInfo.MdPassword = boost::trim_copy(ConfigInfo["MdPassword"]);
      configInfo.Symbol = boost::trim_copy(ConfigInfo["Contract"]);
      if (ConfigInfo.find("Size") != ConfigInfo.end()) {
	  stringstream ss;
	  ss << ConfigInfo["Size"];
	  ss >> configInfo.size;
      }

      if (ConfigInfo.find("TradeHandlerAddr") != ConfigInfo.end()) {
	  configInfo.tradeHandlerAddr = boost::trim_copy(ConfigInfo["TradeHandlerAddr"]);
      }

      if (ConfigInfo.find("TradeHandlerListen") != ConfigInfo.end()) {
	  configInfo.traderHandlerListen = boost::trim_copy(ConfigInfo["TradeHandlerListen"]);
      }
  }
  in.close();
  return configInfo;
}

map<string, FuturesContractInfo> FuturesUtil::initContractInfos() {
  map<string, FuturesContractInfo> configInfos;
  boost::filesystem::path p(CONFIG_ROOT);
  for(auto& entry : boost::make_iterator_range(directory_iterator(p), {})) {
      string filePath = entry.path().string();
      string contract = entry.path().filename().string();

      json_object * jobj = json_object_from_file(filePath.c_str());
      json_object * periodObj = json_object_object_get(jobj, "periods");

      int arrayLength = json_object_array_length(periodObj);
      vector<pair<string, string>> tradingPeriods;
      for(int i = 0; i < arrayLength; ++i) {
	  json_object * jOnePeriod = json_object_array_get_idx(periodObj, i);
	  json_object * jStart = json_object_object_get(jOnePeriod, "start");
	  string start = json_object_get_string(jStart);

	  json_object * jEnd = json_object_object_get(jOnePeriod, "end");
	  string end = json_object_get_string(jEnd);
	  tradingPeriods.push_back(make_pair(start, end));
      }
      configInfos[contract] = {getExchangeFromSymbol(contract), tradingPeriods};
  }
  return configInfos;
}

string FuturesUtil::getExchangeFromSymbol(const string& symbol) {
  string contract = getContractTypeFromSymbol(symbol);
  if(contract == "IF" || contract == "IH" || contract == "IC" || contract == "TF") {
      return CFFEX;
  } else
    return SHFE;
}

string FuturesUtil::getContractTypeFromSymbol(const string& symbol) {
  string s(symbol);
  s.erase(remove_if(s.begin(), s.end(), std::ptr_fun(::isdigit)), s.end());
  boost::to_upper(s);
  return s;
}
