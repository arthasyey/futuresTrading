#ifndef _FUTURES_TRADER_SPI_
#define _FUTURES_TRADER_SPI_

#include "FuturesDataStructures.h"
#include "../ctp/ThostFtdcTraderApi.h"
#include "../include/logging.h"

extern src::severity_logger< severity_level > lg;


class FuturesTraderSpi : public CThostFtdcTraderSpi {
protected:
  FuturesConfigInfo configInfo;
  TThostFtdcFrontIDType	FRONT_ID;
  TThostFtdcSessionIDType	SESSION_ID;
  CThostFtdcTraderApi* pTraderApi;

public:
  int iRequestID;

  FuturesTraderSpi(const FuturesConfigInfo& _configInfo);

  virtual void OnFrontConnected();

  virtual void OnFrontDisconnected();

  void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

  virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

  virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

  virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

  virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

  virtual void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

  virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

  virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

  virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){}

  void OnFrontDisconnected(int nReason) {}

  virtual void OnHeartBeatWarning(int nTimeLapse){}

  virtual void OnRtnOrder(CThostFtdcOrderField *pOrder) {}

  virtual void OnRtnTrade(CThostFtdcTradeField *pTrade){}

  bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);

  bool IsMyOrder(CThostFtdcOrderField *pOrder);

  bool IsTradingOrder(CThostFtdcOrderField *pOrder);

  void ReqQryInvestorPosition();

  void ReqQryInstrument(const char *productIDName = NULL);

  void initLogger() {
    stringstream ss;
    ss << getStrategyId() << "\\" << getStrategyId() << "_%Y%m%d_%N.log";
    ::initLogger(ss.str());
  }

  virtual string getStrategyId() {
    return "Default";
  }

  virtual void ~FuturesTraderSpi() {}
};




#endif // !_FUTURES_TRADER_SPI_
