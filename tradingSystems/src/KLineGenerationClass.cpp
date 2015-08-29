#include <KLineGenerationClass.h>
#include <FuturesDataStructures.h>
#include <FuturesUtil.h>

extern src::severity_logger< severity_level > lg;


void KLineGenerationClass::feedTickData(CThostFtdcDepthMarketDataField * p) {
	preTick = latestTick;
	latestTick = *p;
	generateOneMinuteKLine(p);
}


void KLineGenerationClass::feedOneMinuteKLine(const KLine& oneMinuteKLine) {
	if (dataLoadingNeededBeforeFirstOneMinKLine)
	{
		dataLoadingNeededBeforeFirstOneMinKLine = false;
		string &query = (boost::format("select * from futures.%1%K where period=1 and contract = '%2%' and time < '%3%'") % date % symbol % oneMinuteKLine.time).str();
		ResultSet* res = mysqlConnector.query(query);
		while (res && res->next())
		{
			KLine oneMissedOneMinuteKLine;
			strcpy_s(oneMissedOneMinuteKLine.symbol, symbol.c_str());
			strcpy_s(oneMissedOneMinuteKLine.date, date.c_str());
			strcpy_s(oneMissedOneMinuteKLine.time, res->getString("time").c_str());
			oneMissedOneMinuteKLine.open = res->getDouble("open");
			oneMissedOneMinuteKLine.high = res->getDouble("high");
			oneMissedOneMinuteKLine.low = res->getDouble("low");
			oneMissedOneMinuteKLine.close = res->getDouble("close");
			oneMissedOneMinuteKLine.volume = res->getDouble("volume");
			feedOneMinuteKLine(oneMissedOneMinuteKLine);
		}
	}

	oneMinuteKLines.push_back(oneMinuteKLine);

	BOOST_LOG_SEV(lg, info) << "Feed one minute KLine: " << oneMinuteKLine.toString() << endl;
	for (int i = 0; i < kLineMinutePeriods.size(); ++i)
	{
		int period = kLineMinutePeriods[i];
		int kLineMinute = oneMinuteKLine.getMinute();
		notOneMinuteKLineWorkingSet[i].push_back(oneMinuteKLine);
		if (notOneMinuteKLineWorkingSet[i].size() == period || kLineMinute % period == 0)
		{
			KLine oneLargeKLine(notOneMinuteKLineWorkingSet[i]);
			notOneMinuteKLines[i].push_back(oneLargeKLine);
			OnNotOneMinuteKLineInserted(i);
			notOneMinuteKLineWorkingSet[i].clear();
		}
	}
	OnOneMinuteKLineInserted();
}



void KLineGenerationClass::generateOneMinuteKLine(CThostFtdcDepthMarketDataField *p) {
	vector<string> &timeComponents = FuturesUtil::split(p->UpdateTime, ':');
	/* Aggregate Auction phase of IF */
	string &curMinute = timeComponents[1];
	if (strcmp(p->UpdateTime, "09:14:00") < 0)
		return;
	if (strcmp(p->UpdateTime, "09:14:00") == 0) {
		curMinute = "15";									// The first KLine include data of 9:14:00
	}

	if (isFirstTick)
	{
		firstTickTime = p->UpdateTime;
		isFirstTick = false;
		firstTickMinute = curMinute;
		lastMinute = curMinute;
		BOOST_LOG_SEV(lg, info) << "First tick: " << FuturesUtil::futuresTickToString(p);
		if (firstTickTime == "09:14:00" || firstTickTime == "09:15:00") {
			startedSinceTradingOrDataLoadingPlaned = true;
			lastOneMinuteKLine.initWithTick(p, 0);
		}
	}

	if (!startedSinceTradingOrDataLoadingPlaned)
	{
		if (firstTickMinute == curMinute) {
			BOOST_LOG_SEV(lg, info) << "Bypassing tick: " << FuturesUtil::futuresTickToString(p) << endl;
			return;
		}
		dataLoadingNeededBeforeFirstOneMinKLine = true;
		startedSinceTradingOrDataLoadingPlaned = true;
	}

	/* Start of a new minute, except for the case of 11:30:00 and 15:15:00 */
	if (curMinute != lastMinute) {
		lastMinute = curMinute;
		if (dataLoadingNeededBeforeFirstOneMinKLine && !oneMinuteKLineInitialized) {
			BOOST_LOG_SEV(lg, info) << "Init first KLine with tick: " << FuturesUtil::futuresTickToString(p) << endl;
			lastOneMinuteKLine.initWithTick(p, preTick.Volume);
			oneMinuteKLineInitialized = true;
		}
		else if (strcmp(p->UpdateTime, "11:30:00") == 0 || strcmp(p->UpdateTime, "15:15:00") == 0) {
			lastOneMinuteKLine.feedTick(p, preTick.Volume);
			feedOneMinuteKLine(lastOneMinuteKLine);
			lastOneMinuteKLineAddedSecondPlace = true;
		}
		else {
			if (!lastOneMinuteKLineAddedFirstPlace && !lastOneMinuteKLineAddedSecondPlace) {
				feedOneMinuteKLine(lastOneMinuteKLine);
				lastOneMinuteKLineAddedSecondPlace = false;
			}
			lastOneMinuteKLine = KLine(p, preTick.Volume);
		}
		lastOneMinuteKLineAddedFirstPlace = false;
	}
	else
		lastOneMinuteKLine.feedTick(p, preTick.Volume);

	if (p->UpdateMillisec >= 500 && timeComponents[2] == "59" && strcmp(p->UpdateTime, "11:29:59") != 0 && strcmp(p->UpdateTime, "15:14:59") != 0) {
		feedOneMinuteKLine(lastOneMinuteKLine);
		lastOneMinuteKLineAddedFirstPlace = true;
	}
}