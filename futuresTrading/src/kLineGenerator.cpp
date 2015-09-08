#include <kLineGenerator.h>
#include <FuturesUtil.h>

KLineGenerator::KLineGenerator(const string& _date, const string& _symbol, const vector<int> &_kLineMinutePeriods, bool loadLiveData)
:date(boost::replace_all_copy(_date, "-", "")), symbol(_symbol), kLineMinutePeriods(_kLineMinutePeriods), notOneMinuteKLines(_kLineMinutePeriods.size(), vector<KLine>()),
 notOneMinuteKLineWorkingSet(_kLineMinutePeriods.size(), vector<KLine>()) {
  initOneMinuteKLines(loadLiveData);
  memset(&preTick, 0, sizeof(CThostFtdcDepthMarketDataField));
  memset(&curTick, 0, sizeof(CThostFtdcDepthMarketDataField));
}

void KLineGenerator::initOneMinuteKLines(bool loadLiveData) {
  FuturesContractInfo contractInfo = contractInfos[FuturesUtil::getContractTypeFromSymbol(symbol)];
  cout << "Periods!";
  for(unsigned i = 0; i < contractInfo.tradingPeriods.size(); ++i) {
      string &start = contractInfo.tradingPeriods[i].first;
      string &end = contractInfo.tradingPeriods[i].second;
      cout << "start: " << start << " end: " << end << endl;
      vector<string> kLineTimesInBetween = getKLineTimesInBetween(start, end);
      if(lastOneMinuteKLineTime == "")
	lastOneMinuteKLineTime = kLineTimesInBetween[0];

      for(unsigned i = 0; i < kLineTimesInBetween.size(); ++i) {
	  timeToOneMinuteKLinesMap[kLineTimesInBetween[i]] = KLine(symbol);
	  cout << "prebuild kline: " << kLineTimesInBetween[i] << endl;
      }
  }

  if(loadLiveData) {
      string query = (boost::format("select * from futures.%1%k where period=1 and symbol = '%2%'") % date % symbol).str();
      ResultSet* res = mysqlConnector.query(query);
      while (res && res->next()) {
	  KLine oneMissedOneMinuteKLine;
	  strcpy(oneMissedOneMinuteKLine.symbol, symbol.c_str());
	  strcpy(oneMissedOneMinuteKLine.date, date.c_str());
	  strcpy(oneMissedOneMinuteKLine.time, res->getString("time").c_str());
	  oneMissedOneMinuteKLine.open = res->getDouble("open");
	  oneMissedOneMinuteKLine.high = res->getDouble("high");
	  oneMissedOneMinuteKLine.low = res->getDouble("low");
	  oneMissedOneMinuteKLine.close = res->getDouble("close");
	  oneMissedOneMinuteKLine.volume = res->getDouble("volume");
	  feedOneMinuteKLine(oneMissedOneMinuteKLine);
      }
  }
}

vector<string> KLineGenerator::getKLineTimesInBetween(const string& start, const string& end, bool oneMinuteKLinesPrebuilt) {
  vector<string> ret;
  if(!oneMinuteKLinesPrebuilt) {
      vector<string> startSegments = split(start, ':');
      vector<string> endSegments = split(end, ':');
      int startHour = atoi(startSegments[0].c_str());
      int startMinute = atoi(startSegments[1].c_str());
      int endHour = atoi(endSegments[0].c_str());
      int endMinute = atoi(endSegments[1].c_str());
      for(int h = startHour; h <= endHour; ++h) {
	  int mHead = h == startHour ? startMinute + 1 : 0;
	  int mEnd = h == endHour ? endMinute : 59;
	  for(int m = mHead; m <= mEnd; ++m) {
	      stringstream ss;
	      if(h < 10)
		ss << '0';
	      ss << h << ':';
	      if(m < 10)
		ss << '0';
	      ss << m << ":00";
	      ret.push_back(ss.str());
	  }
      }
  } else {
      for(auto iter = timeToOneMinuteKLinesMap.upper_bound(start); iter != timeToOneMinuteKLinesMap.upper_bound(end); ++iter)
	ret.push_back(iter->first);
  }
  return ret;
}

void KLineGenerator::feedTickData(CThostFtdcDepthMarketDataField * p) {
  preTick = curTick;
  curTick = *p;
  generateOneMinuteKLine(p);
}

void KLineGenerator::feedOneMinuteKLine(const KLine& oneMinuteKLine) {
  timeToOneMinuteKLinesMap[oneMinuteKLine.time] = oneMinuteKLine;
  lastOneMinuteKLineTime = oneMinuteKLine.time;

  // BOOST_LOG_SEV(lg, info) << "Feed one minute KLine: " << oneMinuteKLine.toString() << endl;
  for (unsigned i = 0; i < kLineMinutePeriods.size(); ++i) {
      unsigned period = kLineMinutePeriods[i];
      int kLineMinute = oneMinuteKLine.getMinute();
      notOneMinuteKLineWorkingSet[i].push_back(oneMinuteKLine);
      if (kLineMinute % period == 0) {
	  KLine oneLargeKLine(notOneMinuteKLineWorkingSet[i]);
	  notOneMinuteKLines[i].push_back(oneLargeKLine);
	  OnNotOneMinuteKLineInserted(i);
	  notOneMinuteKLineWorkingSet[i].clear();
      }
  }
  OnOneMinuteKLineInserted();
}

string KLineGenerator::getKLineTimeForTickTime(const string& tickTime) {
  vector<string> segments = split(tickTime, ':');
  int h = atoi(segments[0].c_str());
  int m = atoi(segments[1].c_str());
  int kLineM = m + 1;
  int kLineH = h;
  if (kLineM == 60) {
      kLineM = 0;
      kLineH = h + 1;
  }
  stringstream ss;
  if(kLineH < 10)
    ss << '0';
  ss << kLineH << ':';
  if(kLineM < 10)
    ss << '0';
  ss << kLineM << ":00";
  return ss.str();
}

void KLineGenerator::generateOneMinuteKLine(CThostFtdcDepthMarketDataField *p) {
  string kLineTime = getKLineTimeForTickTime(p->UpdateTime);
  if(FuturesUtil::getExchangeFromSymbol(symbol) == CFFEX && kLineTime == "09:15:00")
    kLineTime = "09:16:00";
  if (timeToOneMinuteKLinesMap.find(kLineTime) != timeToOneMinuteKLinesMap.end()) {
      timeToOneMinuteKLinesMap[kLineTime].feedTick(p, preTick.Volume);

      if (kLineTime != lastOneMinuteKLineTime) {
	  vector<string> kLineTimesInBetween = getKLineTimesInBetween(preTick.UpdateTime, p->UpdateTime, true);
	  lastOneMinuteKLine = timeToOneMinuteKLinesMap[lastOneMinuteKLineTime];
	  for(unsigned i = 0; i < kLineTimesInBetween.size(); ++i) {
	      strcpy(lastOneMinuteKLine.time, kLineTimesInBetween[i].c_str());
	      feedOneMinuteKLine(lastOneMinuteKLine);
	  }
	  lastOneMinuteKLineTime = kLineTime;
      }
  }
}
