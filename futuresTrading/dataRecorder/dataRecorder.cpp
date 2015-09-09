#include "dataRecorder.h"
#include <boost/filesystem.hpp>
#include <utility>

vector<int> kLinePeriods = { 5, 15, 30, 60 };

MyTraderSpi::MyTraderSpi(const FuturesConfigInfo& _config) : config(_config) {
  pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
  pTraderApi->SubscribePublicTopic(THOST_TERT_QUICK);
  pTraderApi->SubscribePrivateTopic(THOST_TERT_QUICK);
  pTraderApi->RegisterSpi(this);
  pTraderApi->RegisterFront(const_cast<char*>(config.TraderFrontAddr.c_str()));
  LOG_BOOST << config.toString();
  pTraderApi->Init();
}

void MyTraderSpi::OnFrontConnected() {
  CThostFtdcReqUserLoginField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, config.BrokerId.c_str());
  strcpy(req.UserID, config.UserId.c_str());
  strcpy(req.Password, config.Password.c_str());

  int iResult = pTraderApi->ReqUserLogin(&req, ++requestId);
  LOG_BOOST << "--->>>" << __PRETTY_FUNCTION__ << ((iResult == 0) ? "success" : "failure") << endl;
}

void MyTraderSpi::requestQryInstrument(const string& exchange) {
  struct CThostFtdcQryInstrumentField reqInstrument;
  memset(&reqInstrument, 0, sizeof(reqInstrument));
  strcpy(reqInstrument.ExchangeID, exchange.c_str());
  lastExchangeStr = exchange;

  int ret = -2;
  while(UNDER_CTP_FLOW_CONTROL(ret)) {
  sleep(1);
    ret = pTraderApi->ReqQryInstrument(&reqInstrument, ++requestId);
    LOG_BOOST << " " << exchange << " return" << ret;
  }
}

void MyTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  LOG_BOOST << "user :: " << nRequestID << " :: "
      << pRspUserLogin->TradingDay << " :: "
      << pRspUserLogin->LoginTime << " :: "
      << pRspUserLogin->BrokerID << " :: "
      << pRspUserLogin->UserID << " :: "
      << pRspUserLogin->SystemName << endl;

  requestQryInstrument(CFFEX);
}

void MyTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  if (pRspInfo == NULL && pInstrument != NULL)
  {
    if (pInstrument->ProductClass == THOST_FTDC_PC_Futures)
    {
      cerr << "get: " << pInstrument->InstrumentID << endl;
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
    if(lastExchangeStr == CFFEX )
      requestQryInstrument(SHFE);
    else if (lastExchangeStr == SHFE)
      requestQryInstrument(DCE);
    else if (lastExchangeStr == DCE)
      requestQryInstrument(CZCE);
    else
      cerr << "finish req instrument ... " << endl;
    return;
  }
}


DataRecorder::DataRecorder(const FuturesConfigInfo& config) : traderSpi(config), configInfo(config) {
  initTables();
  pMdApi = CThostFtdcMdApi::CreateFtdcMdApi();
  pMdApi->RegisterSpi(this);
  pMdApi->Init();
  pMdApi->RegisterFront(const_cast<char*>(configInfo.MdFrontAddr.c_str()));
}

void DataRecorder::OnFrontDisconnected(int nReason) {
  LOG_BOOST << " reason code: " << nReason;
  pMdApi->RegisterFront(const_cast<char*>(configInfo.MdFrontAddr.c_str()));
}

void DataRecorder::OnFrontConnected() {
  CThostFtdcReqUserLoginField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, configInfo.MdBrokerId.c_str());
  strcpy(req.UserID, configInfo.MdUserId.c_str());
  strcpy(req.Password, configInfo.MdPassword.c_str());
  int iResult = pMdApi->ReqUserLogin(&req, 0);
  cerr << "--->>>" << __PRETTY_FUNCTION__ << ((iResult == 0) ? "success" : "failure") << endl;
}

void DataRecorder::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  sleep(20);

  LOG_BOOST << "subscribe to symbols: " << concatenate(traderSpi.instruments, ",") << endl;
  int count = traderSpi.instruments.size();
  char** pInstruments = new char*[count];
  for (int i = 0; i < count; ++i)
  {
    string& instrument = traderSpi.instruments[i];
    pInstruments[i] = new char[20];
    strcpy(pInstruments[i], instrument.c_str());

    string directory = DATA_ROOT + instrument + "/";
    boost::filesystem::create_directory(directory);

    string fileName = directory + FuturesUtil::getCurrentDateString() + ".csv";
    shared_ptr<ofstream> tickOutputStream(new ofstream(fileName.c_str(), ios_base::out | ios_base::app));
    tickOutputStream->precision(8);
    if(!boost::filesystem::exists(fileName.c_str()))
      *tickOutputStream << "updateTime,updateMillis,lastPrice,avgPrice,bidPrice1,askPrice1,bidVolume1,askVolume1,volume" << endl;

    tickOutputStreams[instrument] = tickOutputStream;
    accuVolumes[instrument] = 0;
    lastAggregateAvgPrice[instrument] = 0;
    klineGenerators[instrument] = shared_ptr<MyKlineGenerator>(new MyKlineGenerator(instrument));
  }
  pMdApi->SubscribeMarketData(pInstruments, count);
  for (int i = 0; i < count; ++i)
    delete pInstruments[i];
  delete pInstruments;
}


void DataRecorder::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {
  string instrument = pDepthMarketData->InstrumentID;

  int newVolume = pDepthMarketData->Volume;
  double newAggAvgPrice = pDepthMarketData->AveragePrice;

  int volume = newVolume - accuVolumes[instrument];
  double avgPrice = volume == 0 ? pDepthMarketData->LastPrice : (newVolume * newAggAvgPrice - lastAggregateAvgPrice[instrument] * accuVolumes[instrument]) / volume;

  *tickOutputStreams[instrument] << pDepthMarketData->UpdateTime << ","
      << pDepthMarketData->UpdateMillisec << ","
      << pDepthMarketData->LastPrice << ","
      << avgPrice << ","
      << pDepthMarketData->BidPrice1 << ","
      << pDepthMarketData->AskPrice1 << ","
      << pDepthMarketData->BidVolume1 << ","
      << pDepthMarketData->AskVolume1 << ","
      << volume << endl;

  lastAggregateAvgPrice[instrument] = newAggAvgPrice;
  accuVolumes[instrument] = newVolume;

  klineGenerators[instrument]->feedTickData(pDepthMarketData);
}

MyKlineGenerator::MyKlineGenerator(const string& contract) : KLineGenerator(FuturesUtil::getCurrentDateString(), contract, kLinePeriods, false){}

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
                 % lastKLine.volume).str();
  mysqlConnector.executeUpdate(query);
}


void DataRecorder::initTables() {
  string query = (boost::format(CREATE_KLINE_TABLE_QUERY) % FuturesUtil::getCurrentDateString()).str();
  mysqlConnector.executeUpdate(query);
}


void DataRecorder::finalize() {
  for(auto i = klineGenerators.begin(); i != klineGenerators.end(); ++i) {
      i->second->OnOneMinuteKLineInserted();
  }
}
