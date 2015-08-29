#include "../include/FuturesUtil.h"
#include "../include/logging.h"
#include <fstream>
#include <sstream>
#include <limits>
#include <chrono>


bool floatNumberLess(double first, double second) {
  return first < second - numeric_limits<double>::epsilon();
}

bool floatNumberEqual(double first, double second) {
  return abs(first - second) <= numeric_limits<double>::epsilon();	
}

bool floatNumberLessEqual(double first, double second) {
  return first - second <= numeric_limits<double>::epsilon();
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
  ss << (now->tm_year + 1900) << '-'
    << (now->tm_mon + 1) << '-'
    <<  now->tm_mday
    << endl;
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
      return "全部成交";

    case THOST_FTDC_OST_PartTradedQueueing:
      return "部分成交还在队列中";

    case THOST_FTDC_OST_PartTradedNotQueueing:
      return "部分成交不在队列中";

    case THOST_FTDC_OST_NoTradeQueueing:
      return "未成交还在队列中";

    case THOST_FTDC_OST_NoTradeNotQueueing:
      return "未成交不在队列中";

    case THOST_FTDC_OST_Canceled:
      return "撤单";

    case THOST_FTDC_OST_Unknown:
      return "未知";

    case THOST_FTDC_OST_NotTouched:
      return "尚未触发";

    case THOST_FTDC_OST_Touched:
      return "已触发";

    default:
      return "NA";
  }
}


string FuturesUtil::orderSubmissionStatusToString(TThostFtdcOrderSubmitStatusType status) {
  switch (status)
  {
    case THOST_FTDC_OSS_InsertSubmitted:
      return "已经提交";
    case THOST_FTDC_OSS_CancelSubmitted:
      return "撤单已经提交";
    case THOST_FTDC_OSS_ModifySubmitted:
      return  "修改已经提交";
    case THOST_FTDC_OSS_Accepted:
      return  "已经接受";
    case THOST_FTDC_OSS_InsertRejected:
      return  "报单已经被拒绝";
    case THOST_FTDC_OSS_CancelRejected:
      return  "撤单已经被拒绝";
    case THOST_FTDC_OSS_ModifyRejected:
      return  "改单已经被拒绝";

    default:
      return "NA";
  }
}


string FuturesUtil::orderActionStatusToString(TThostFtdcOrderActionStatusType status) {
  switch (status)
  {
    case THOST_FTDC_OAS_Submitted:
      return "已经提交";
    case THOST_FTDC_OAS_Accepted:
      return  "已经接受";
    case THOST_FTDC_OAS_Rejected:
      return  "已经被拒绝";

    default:
      return "NA";
  }
}



FuturesConfigInfo FuturesUtil::LoadConfigureFile(const string &filename){
  FuturesConfigInfo configInfo;
  map<string, string> ConfigInfo;
  ifstream in;
  in.open(filename, ios::in);
  if (in.is_open()){
    string tmpStr;
    while (getline(in, tmpStr))
    {
      int pos = tmpStr.find('=');
      string strName = tmpStr.substr(0, pos);
      string strValue = tmpStr.substr(pos + 1, tmpStr.length());
      ConfigInfo[strName] = strValue;
    }
    configInfo.TraderFrontAddr = ConfigInfo["TraderFrontAddr"];
    configInfo.MdFrontAddr = ConfigInfo["MdFrontAddr"];
    configInfo.BrokerId = ConfigInfo["BrokerId"];
    configInfo.UserId = ConfigInfo["UserId"];
    configInfo.Password = ConfigInfo["Password"];
    configInfo.MdBrokerId = ConfigInfo["MdBrokerId"];
    configInfo.MdUserId = ConfigInfo["MdUserId"];
    configInfo.MdPassword = ConfigInfo["MdPassword"];
    configInfo.Contract = ConfigInfo["Contract"];
    if (ConfigInfo.find("Size") != ConfigInfo.end()) {
      stringstream ss;
      ss << ConfigInfo["Size"];
      ss >> configInfo.Size;
    }

    if (ConfigInfo.find("TradingServerAddr") != ConfigInfo.end()) {
      configInfo.TradingServerAddr = ConfigInfo["TradingServerAddr"];
    }

    if (ConfigInfo.find("TradingServerListen") != ConfigInfo.end()) {
      configInfo.TradingServerListen = ConfigInfo["TradingServerListen"];
    }

    if (ConfigInfo.find("Size") != ConfigInfo.end()) {
      stringstream ss;
      ss << ConfigInfo["Size"];
      ss >> configInfo.Size;
    }
  }
  in.close();
  return configInfo;
}
