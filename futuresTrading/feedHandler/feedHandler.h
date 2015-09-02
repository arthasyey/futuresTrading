#ifndef _FEED_HANDLER_
#define _FEED_HANDLER_

#include <ThostFtdcMdApi.h>
#include <FuturesUtil.h>
#include <zhelpers.hpp>

class FeedHandler : public CThostFtdcMdSpi {
protected:
  FuturesConfigInfo config;
  zmq::context_t context;
  zmq::socket_t feedPublisher;
  CThostFtdcMdApi* pMdApi;

public:
  FeedHandler(const FuturesConfigInfo& _configInfo);

  virtual void OnFrontConnected();

  virtual void OnFrontDisconnected(int nReason);

  virtual void OnHeartBeatWarning(int nTimeLapse){};

  virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);// {}

  virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

  virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

  virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    cerr << "--->>> " << "FuturesMdSpi::OnRspSubMarketData: " << pSpecificInstrument->InstrumentID << endl;
  }

  virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

  virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

  int SubscribeMarketData(const vector<string>& instruments);

  int SubscribeMarketData(const string& instrument);

  virtual ~FeedHandler() {}
};




#endif // !_TRADER_SPI_
