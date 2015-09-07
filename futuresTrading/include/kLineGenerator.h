#ifndef _KLINE_GENERATION_CLASS_
#define _KLINE_GENERATION_CLASS_

#include <FuturesUtil.h>
#include <boost/algorithm/string.hpp>
#include <mysqlConnector.h>
#include <map>

class KLineGenerator {
protected:
  string date;
  string symbol;
  CThostFtdcDepthMarketDataField preTick;
  CThostFtdcDepthMarketDataField curTick;
  KLine lastOneMinuteKLine;
  string lastOneMinuteKLineTime = "";

  vector<int> kLineMinutePeriods;
  vector<vector<KLine>> notOneMinuteKLines;
  vector<vector<KLine>> notOneMinuteKLineWorkingSet;
  map<string, KLine> timeToOneMinuteKLinesMap;

  void generateOneMinuteKLine(CThostFtdcDepthMarketDataField *p);
  void feedOneMinuteKLine(const KLine& oneMinuteKLine);
  string getKLineTimeForTickTime(const string& tickTime);
  vector<string> getKLineTimesInBetween(const string& start, const string& end, bool built = false);
public:
  KLineGenerator(const string& _date, const string& _symbol, const vector<int> &_kLineMinutePeriods);

  void initOneMinuteKLines();

  void feedTickData(CThostFtdcDepthMarketDataField * p);
  virtual void OnOneMinuteKLineInserted() {};
  virtual void OnNotOneMinuteKLineInserted(int periodIndex) {};
  virtual void OnPastKLineFilled(){};

  virtual ~KLineGenerator() {}
};

#endif // !_KLINE_GENERATION_CLASS_
