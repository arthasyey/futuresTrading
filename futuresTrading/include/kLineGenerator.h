#ifndef _KLINE_GENERATION_CLASS_
#define _KLINE_GENERATION_CLASS_

#pragma once
#include <FuturesUtil.h>
#include <boost/algorithm/string.hpp>
#include "../include/mysqlConnector.h"
#include "../include/logging.h"
using namespace std;
using namespace sql;


class KLineGenerator {
private:
  void generateOneMinuteKLine(CThostFtdcDepthMarketDataField *p);
  bool oneMinuteKLineInitialized = false;
  bool dataLoadingNeededBeforeFirstOneMinKLine = false;
  bool lastOneMinuteKLineAddedSecondPlace = false;
  bool lastOneMinuteKLineAddedFirstPlace = false;
  bool firstTickInAfternoonProcessed = true;
  string firstTickTime;
  string firstTickMinute;
  string lastMinute;
  void feedOneMinuteKLine(const KLine& oneMinuteKLine);

protected:
  bool startedSinceTradingOrDataLoadingPlaned = false;
  string date;
  string symbol;
  vector<int> kLineMinutePeriods;
  bool stopped = false;
  MySqlConnector mysqlConnector;
  bool isFirstTick = true;
  KLine lastOneMinuteKLine;
  vector<vector<KLine>> notOneMinuteKLines;
  vector<vector<KLine>> notOneMinuteKLineWorkingSet;
  vector<KLine> oneMinuteKLines;
  CThostFtdcDepthMarketDataField preTick;
  CThostFtdcDepthMarketDataField latestTick;

public:
  KLineGenerator(const string& _date, const string& _symbol, const vector<int> &_kLineMinutePeriods)
    :date(boost::replace_all_copy(_date, "-", "")), symbol(_symbol), kLineMinutePeriods(_kLineMinutePeriods), notOneMinuteKLines(_kLineMinutePeriods.size(), vector<KLine>()),
    notOneMinuteKLineWorkingSet(_kLineMinutePeriods.size(), vector<KLine>()){
    memset(&preTick, 0, sizeof(CThostFtdcDepthMarketDataField));
    memset(&latestTick, 0, sizeof(CThostFtdcDepthMarketDataField));
  }

  void feedTickData(CThostFtdcDepthMarketDataField * p);
  virtual void OnOneMinuteKLineInserted() {};
  virtual void OnNotOneMinuteKLineInserted(int periodIndex) {};
  virtual void OnPastKLineFilled(){};

  virtual ~KLineGenerator() {}

};

#endif // !_KLINE_GENERATION_CLASS_
