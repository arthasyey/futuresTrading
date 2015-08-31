#include "dataRecorder.h"
#include <boost/filesystem.hpp>
#include <utility>


vector<int> kLinePeriods = { 5, 15, 30, 60 };

MySqlConnector DataRecorder::mySqlConnector;

MyKlineGenerator::MyKlineGenerator(const string& contract) : KLineGenerator(FuturesUtil::getCurrentDateString(), contract, kLinePeriods){}

MyTraderSpi::MyTraderSpi(const FuturesConfigInfo& _config) : config(_config) {
  pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
  pTraderApi->SubscribePublicTopic(THOST_TERT_QUICK);
  pTraderApi->SubscribePrivateTopic(THOST_TERT_QUICK);
  pTraderApi->RegisterSpi(this);
  pTraderApi->RegisterFront(const_cast<char*>(config.TraderFrontAddr.c_str()));
  BOOST_LOG_SEV(lg, info) << __func__ << config.toString();
  pTraderApi->Init();
}

void MyTraderSpi::OnFrontConnected() {
  CThostFtdcReqUserLoginField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, config.BrokerId.c_str());
  strcpy(req.UserID, config.UserId.c_str());
  strcpy(req.Password, config.Password.c_str());

  int iResult = pTraderApi->ReqUserLogin(&req, 1);
  BOOST_LOG_SEV(lg, info) << "--->>>" << __func__ << ((iResult == 0) ? "success" : "failure") << endl;
}

void MyTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  BOOST_LOG_SEV(lg, info) << "user :: " << nRequestID << " :: "
      << pRspUserLogin->TradingDay << " :: "
      << pRspUserLogin->LoginTime << " :: "
      << pRspUserLogin->BrokerID << " :: "
      << pRspUserLogin->UserID << " :: "
      << pRspUserLogin->SystemName << endl;

  struct CThostFtdcQryInstrumentField reqInstrument;
  memset(&reqInstrument, 0, sizeof(reqInstrument));
  strcpy(reqInstrument.ExchangeID, "CFFEX");
  pTraderApi->ReqQryInstrument(&reqInstrument, 2);
}

void MyTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  if (pRspInfo == NULL)
  {
    if (pInstrument->ProductClass == THOST_FTDC_PC_Futures)
    {
      cerr << "get" << pInstrument->InstrumentID << endl;
      instruments.push_back(pInstrument->InstrumentID);
    }
  }
  else
  {
    cerr << "ERROR : " << pRspInfo->ErrorID << " : " << pRspInfo->ErrorMsg << endl;
    return;
  }
  if (bIsLast)
  {
    cerr << "finish req instrument ... " << endl;
    return;
  }
}


DataRecorder::DataRecorder(const FuturesConfigInfo& config) : traderSpi(config), configInfo(config) {
  initTables();
  pMdApi = CThostFtdcMdApi::CreateFtdcMdApi();
  pMdApi->RegisterSpi(this);
  pMdApi->Init();
}

void DataRecorder::OnFrontDisconnected(int nReason) {
  BOOST_LOG_SEV(lg, info) << __func__ << " reason code: " << nReason;
  pMdApi->RegisterFront(const_cast<char*>(configInfo.MdFrontAddr.c_str()));
}

void DataRecorder::OnFrontConnected() {
  CThostFtdcReqUserLoginField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, configInfo.MdBrokerId.c_str());
  strcpy(req.UserID, configInfo.MdUserId.c_str());
  strcpy(req.Password, configInfo.MdPassword.c_str());
  int iResult = pMdApi->ReqUserLogin(&req, 0);
  cerr << "--->>>" << __FUNCTION__ << ((iResult == 0) ? "success" : "failure") << endl;
}

void DataRecorder::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  sleep(15);

  cerr << "subscribe to symbols: " << concatenate(traderSpi.instruments, ",") << endl;
  int count = traderSpi.instruments.size();
  char** pInstruments = new char*[count];
  for (int i = 0; i < count; ++i)
  {
    string& instrument = traderSpi.instruments[i];
    pInstruments[i] = new char[20];
    strcpy(pInstruments[i], instrument.c_str());

    string directory = DATA_ROOT + instrument + "/";
    boost::filesystem::create_directory(directory);

    shared_ptr<ofstream> tickOutputStream(new ofstream(directory + FuturesUtil::getCurrentDateString() + ".csv"));
    tickOutputStream->precision(8);
    *tickOutputStream << "updateTime,updateMillis,lastPrice,avgPrice,bidPrice1,askPrice1,bidVolume1,askVolume1,volume" << endl;

    tickOutputStreams[instrument] = tickOutputStream;
    accuVolumes[instrument] = 0;
    accuTurnovers[instrument] = 0;
    klineGenerators[instrument] = shared_ptr<MyKlineGenerator>(new MyKlineGenerator(instrument));
  }
  pMdApi->SubscribeMarketData(pInstruments, count);
  for (int i = 0; i < count; ++i)
    delete pInstruments[i];
  delete pInstruments;
}


void DataRecorder::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {
  cerr <<__FUNCTION__ <<endl;
  string instrument = pDepthMarketData->InstrumentID;

  int newVolume = pDepthMarketData->Volume;
  int volume = newVolume - accuVolumes[instrument];
  double newTurnOver = pDepthMarketData->Turnover;
  double avgPrice = (newTurnOver - accuTurnovers[instrument]) / volume / 300;

  *tickOutputStreams[instrument] << pDepthMarketData->UpdateTime << ","
      << pDepthMarketData->UpdateMillisec << ","
      << pDepthMarketData->LastPrice << ","
      << avgPrice << ","
      << pDepthMarketData->BidPrice1 << ","
      << pDepthMarketData->AskPrice1 << ","
      << pDepthMarketData->BidVolume1 << ","
      << pDepthMarketData->AskVolume1 << ","
      << volume << endl;

  accuTurnovers[instrument] = newTurnOver;
  accuVolumes[instrument] = newVolume;

  klineGenerators[instrument]->feedTickData(pDepthMarketData);
}

void MyKlineGenerator::OnOneMinuteKLineInserted() {
  string query = (boost::format(INSERT_KLINE_QUERY)
  % lastOneMinuteKLine.date
  % lastOneMinuteKLine.symbol
  % 1
  % lastOneMinuteKLine.time
  % lastOneMinuteKLine.open
  % lastOneMinuteKLine.high
  % lastOneMinuteKLine.low
  % lastOneMinuteKLine.close
  % lastOneMinuteKLine.volume).str();
  mysqlConnector.executeUpdate(query);
}


void MyKlineGenerator::OnNotOneMinuteKLineInserted(int periodIndex) {
  KLine &lastKLine = notOneMinuteKLines[periodIndex].back();
  string query = (boost::format(INSERT_KLINE_QUERY)
  % lastKLine.date
  % lastKLine.symbol
  % kLinePeriods[periodIndex]
                 % lastKLine.time
                 % lastKLine.open
                 % lastKLine.high
                 % lastKLine.low
                 % lastKLine.close
                 % lastKLine.volume
                 % lastKLine.tickCount).str();
  mysqlConnector.executeUpdate(query);
}


void DataRecorder::initTables() {
  string query = (boost::format(CREATE_KLINE_TABLE_QUERY) % FuturesUtil::getCurrentDateString()).str();
  mySqlConnector.executeUpdate(query);
}
