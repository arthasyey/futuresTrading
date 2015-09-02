#ifndef _FUTURES_TRADING_SERVER_
#define _FUTURES_TRADING_SERVER_

#include "../include/FuturesUtil.h"
#include "../include/zhelpers.hpp"
#include "../ctp/ThostFtdcTraderApi.h"

class TradeHandler : public CThostFtdcTraderSpi {
private:
  FuturesConfigInfo configInfo;
  zmq::context_t context;
  zmq::socket_t requestPuller;
  zmq::socket_t responsePublisher;

  TThostFtdcFrontIDType	FRONT_ID;
  TThostFtdcSessionIDType	SESSION_ID;
  CThostFtdcTraderApi* pTraderApi;

  int iRequestID = 0;
public:

  TradeHandler(const FuturesConfigInfo& _configInfo);

  virtual void OnFrontConnected();

  virtual void OnFrontDisconnected(int nReason);

  void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

  virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

  virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

  virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

  virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

  virtual void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

  bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);

  bool IsTradingOrder(CThostFtdcOrderField *pOrder);

  virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

  void ReqQryInvestorPosition();
  void ReqQryInstrument(const char *productIDName = NULL);

  virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);
  virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

  virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
  virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);

  void run();

  virtual ~TradeHandler(){}
};

#endif
