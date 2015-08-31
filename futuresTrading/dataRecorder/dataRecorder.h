#ifndef _DATA_RECORDER_
#define _DATA_RECORDER_

#include "../ctp/ThostFtdcMdApi.h"
#include "../ctp/ThostFtdcTraderApi.h"
#include <kLineGenerator.h>
#include <mysqlConnector.h>
#include <fstream>
#include <memory>

extern src::severity_logger< severity_level > lg;

#define INSERT_KLINE_QUERY "insert into futures.%1%k values ('%2%', %3%, '%4%', %5%, %6%, %7%, %8%, %9%)"

#define CREATE_KLINE_TABLE_QUERY "CREATE TABLE if not exists `futures`.`%1%k` (		\
    `symbol` VARCHAR(10) NOT NULL,					 \
    `period` int NOT NULL,						  \
    `time` VARCHAR(12) NOT NULL,							   \
    `open` DOUBLE NULL,										 \
    `high` DOUBLE NULL,									 \
    `low` DOUBLE NULL,									  \
    `close` DOUBLE NULL,								 \
    `volume` INT NULL,								  \
    INDEX `idx1` (`symbol` ASC),								 \
    INDEX `idx2` (period ASC),									 \
    INDEX `idx3` (time ASC))"

class MyKlineGenerator : public KLineGenerator {
public:
  MyKlineGenerator(const string& contract);
  void OnOneMinuteKLineInserted();
  void OnNotOneMinuteKLineInserted(int periodIndex);
};


class MyTraderSpi : public CThostFtdcTraderSpi {
  CThostFtdcTraderApi *pTraderApi;
  FuturesConfigInfo config;
public:
  MyTraderSpi(const FuturesConfigInfo& config);
  virtual void OnFrontConnected();
  virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  vector<string> instruments;
  virtual ~MyTraderSpi() {}
};


class DataRecorder : public CThostFtdcMdSpi {
private:
  CThostFtdcMdApi *pMdApi;
  MyTraderSpi traderSpi;
  FuturesConfigInfo configInfo;
public:
  static MySqlConnector mySqlConnector;

  void initTables();
  DataRecorder(const FuturesConfigInfo& config);
  virtual void OnFrontConnected();
  virtual void OnFrontDisconnected(int nReason);
  virtual void OnHeartBeatWarning(int nTimeLapse){};
  virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);// {}
  void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
  void OnOneMinuteKLineInserted();
  void OnNotOneMinuteKLineInserted(int periodIndex);
  void static processHistoryData(string date, string symbol);

  map<string, shared_ptr<ofstream>> tickOutputStreams;
  map<string, shared_ptr<MyKlineGenerator>> klineGenerators;
  map<string, int> accuVolumes;
  map<string, double> accuTurnovers;

  virtual ~DataRecorder() {}
};




#endif
