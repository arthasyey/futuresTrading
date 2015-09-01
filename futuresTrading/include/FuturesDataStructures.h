#ifndef _FUTUERS_DATA_STRUCTURES_
#define _FUTUERS_DATA_STRUCTURES_

#include <string>
#include <iostream>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include "../ctp/ThostFtdcUserApiStruct.h"

using namespace std;

#define DATA_ROOT		 string("/marketData/")

#define UNDER_CTP_FLOW_CONTROL(ret) (ret == -2 || ret == -3)

#define CFFEX "CFFEX"
#define SHFEX "SHFE"
#define DCEX  "DCE"
#define CZCEX "CZCE"

enum Direction
{
  Long,
  Short
};


enum ORDER_DIRECTION {
  OPEN_LONG = 0,
  OPEN_SHORT = 1,
  CLOSE_LONG = 11,
  CLOSE_SHORT = 10
};

struct CInvestorPosition{
  CThostFtdcInvestorPositionField LongPosition;
  CThostFtdcInvestorPositionField ShortPosition;
};

struct FuturesConfigInfo {
  string TraderFrontAddr;
  string MdFrontAddr;
  string BrokerId, UserId, Password;
  string MdBrokerId, MdUserId, MdPassword;
  string Contract;
  string tradeHandlerAddr;
  string traderHandlerListen;
  string feedHandlerAdddr;
  string simDate;
  int size;

  FuturesConfigInfo(const string& traderFrontAddr, const string& mdFrontAddr, const string& brokerId, const string& userId, const string& password, int size = 1) :
    TraderFrontAddr(traderFrontAddr), MdFrontAddr(mdFrontAddr), BrokerId(brokerId), UserId(userId), Password(password), MdBrokerId(brokerId), MdUserId(userId), MdPassword(password), size(size){}

  FuturesConfigInfo(const string& traderFrontAddr, const string& mdFrontAddr, const string& brokerId, const string& userId, const string& password,
            const string& mdBrokerId, const string& mdUserId, const string& mdPassword, int size = 1) : TraderFrontAddr(traderFrontAddr), MdFrontAddr(mdFrontAddr),
            BrokerId(brokerId), UserId(userId), Password(password), MdBrokerId(mdBrokerId), MdUserId(mdUserId), MdPassword(mdPassword), size(size){}

  FuturesConfigInfo() :TraderFrontAddr("tcp://asp-sim2-front1.financial-trading-platform.com:26205"),
    MdFrontAddr("tcp://asp-sim2-md1.financial-trading-platform.com:26213"), BrokerId("2030"), UserId("80489"), Password("888888"),
    MdBrokerId("2030"), MdUserId("80489"), MdPassword("888888"), Contract("IF1412"), tradeHandlerAddr("inproc://localhost:9995"),
    traderHandlerListen("inproc://localhost:9996"), feedHandlerAdddr("inproc://localhost:9997"), size(1) {
  }

  FuturesConfigInfo(const string& _symbol, const string& _simDate) : Contract(_symbol), simDate(_simDate), size(1) {}

  string toString() {
   stringstream ss;
   ss << "[TraderFrontAddr: " << TraderFrontAddr
    << ", MdFrontAddr: " << MdFrontAddr
    << ", BrokerId: " << BrokerId
    << ", UserId: " << UserId
    << ", Password: " << Password
    << ", MdBrokerId: " << MdBrokerId
    << ", MdUserId: " << MdUserId
    << ", MdPassword: " << MdPassword
    << ", Contract: " << Contract
    << ", tradeHandlerAddr: " << tradeHandlerAddr
    << ", tradeHandlerListen: " << traderHandlerListen
    << ", feedHandlerAddr: " << feedHandlerAdddr
    << ", simDate: " << simDate
    << ", size: " << size << ']';
   return ss.str();
  }
};


struct SecurityCtpConfigInfo {
  string TraderFrontAddr;
  string MdFrontAddr;
  string BrokerId, UserId, Password;
  string MdBrokerId, MdUserId, MdPassword;
  string Contract;
  int Size;

  SecurityCtpConfigInfo(const string& traderFrontAddr, const string& mdFrontAddr, const string& brokerId, const string& userId, const string& password, int size = 1) :
    TraderFrontAddr(traderFrontAddr), MdFrontAddr(mdFrontAddr), BrokerId(brokerId), UserId(userId), Password(password), MdBrokerId(brokerId), MdUserId(userId), MdPassword(password), Size(size){}

  SecurityCtpConfigInfo(const string& traderFrontAddr, const string& mdFrontAddr, const string& brokerId, const string& userId, const string& password,
    const string& mdBrokerId, const string& mdUserId, const string& mdPassword, int size = 1) : TraderFrontAddr(traderFrontAddr), MdFrontAddr(mdFrontAddr),
    BrokerId(brokerId), UserId(userId), Password(password), MdBrokerId(mdBrokerId), MdUserId(mdUserId), MdPassword(mdPassword), Size(size){}

  SecurityCtpConfigInfo() :TraderFrontAddr("tcp://14.17.75.31:6507"),
    MdFrontAddr("tcp://14.17.75.31:6507"), BrokerId("2011"), UserId("020090005757"), Password("1233640"),
    MdBrokerId("2011"), MdUserId("020090005757"), MdPassword("1233640"), Contract("IF1412") {
  }

};




struct KLine{
  TThostFtdcInstrumentIDType symbol;
  TThostFtdcDateType date;
  TThostFtdcTimeType time;
  TThostFtdcPriceType	open;
  TThostFtdcPriceType	high;
  TThostFtdcPriceType	low;
  TThostFtdcPriceType	close;
  TThostFtdcVolumeType volume;
  int tickCount;

  KLine() :high(0), low(100000){}

  string toString() const {
    stringstream ss;
    ss << "[" << symbol << ", dateTime: " << date << ' ' << time << " open: " << open << " high: " << high << " low: " << low << " close: " << close << " volume: " << volume << "]";
    return ss.str();
  }

  int getMinute() const {
    stringstream ss(time);
    vector<string> items;
    string item;
    while (getline(ss, item, ':'))
      items.push_back(item);
    stringstream converterSS;
    int minute;
    converterSS << items[1];
    converterSS >> minute;
    return minute;
  }

  KLine(CThostFtdcDepthMarketDataField * p, int lastTickVolume) {
    initWithTick(p, lastTickVolume);
  }

  KLine(const vector<KLine>& kLines){
    const KLine& firstKLine = kLines.front();
    const KLine& lastKLine = kLines.back();
    high = 0;
    low = 1000000;
    volume = 0;
    tickCount = 0;

    strcpy(symbol, firstKLine.symbol);
    strcpy(date, firstKLine.date);
    strcpy(time, lastKLine.time);
    open = firstKLine.open;
    close = lastKLine.close;

    for (unsigned i = 0; i < kLines.size(); ++i)
    {
      high = max(high, kLines[i].high);
      low = min(low, kLines[i].low);
      volume += kLines[i].volume;
      tickCount += kLines[i].tickCount;
    }
  }


  void initWithTick(CThostFtdcDepthMarketDataField * p, int lastTickVolume) {
    strcpy(symbol, p->InstrumentID);
    strcpy(date, p->TradingDay);
    strcpy(symbol, p->InstrumentID);
    strcpy(date, p->TradingDay);
    open = high = low = close = p->LastPrice;
    volume = p->Volume - lastTickVolume;

    string updateTime(p->UpdateTime);
    int firstColon = updateTime.find_first_of(":");
    int secondColon = updateTime.find_first_of(':', firstColon + 1);
    int minuteInt = boost::lexical_cast<int>(updateTime.substr(firstColon + 1, secondColon - firstColon - 1)) + 1;
    int hourInt = boost::lexical_cast<int>(updateTime.substr(0, firstColon));

    if (hourInt == 9 && (minuteInt == 15 || minuteInt == 14))
      minuteInt = 16;

    if (minuteInt == 60) {
      hourInt++;
      minuteInt = 0;
    }
    stringstream ss;
    if (hourInt < 10)
      ss << "0";
    ss << hourInt << ':';
    if (minuteInt < 10)
      ss << "0";
    ss << minuteInt << ":00";
    strcpy(time, ss.str().c_str());
    tickCount = 1;
  }

  void feedTick(CThostFtdcDepthMarketDataField * p, int lastTickVolume) {
    close = p->LastPrice;
    volume += (p->Volume - lastTickVolume);
    ++tickCount;
    high = max(high, p->LastPrice);
    low = min(low, p->LastPrice);
  }
};


struct RejectInfo {
  string orderRef;
  string reason;

  RejectInfo(const string& _orderRef, const string& _reason) : orderRef(_orderRef), reason(_reason) {}
};

enum MSG_TYPE {
  REQ_ORDER_INSERT,
  REQ_ORDER_ACTION,

  REP_RTN_TRADE,
  REP_RTN_ORDER,

  REP_RSP_ORDER_ACTION,
  REP_RSP_ORDER_INSERT,

  REP_ERR_RTN_ORDER_ACTION,
  REP_ERR_RTN_ORDER_INSERT,

  MSG_FEED
};

#endif
