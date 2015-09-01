#include "../include/FuturesTraderSpi.h"

FuturesTraderSpi::FuturesTraderSpi(const FuturesConfigInfo& _configInfo) : configInfo(_configInfo), iRequestID(0) {}

void FuturesTraderSpi::OnFrontDisconnected() {

}

void FuturesTraderSpi::OnFrontConnected()
{
  initLogger();
  CThostFtdcReqUserLoginField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, configInfo.BrokerId.c_str());
  strcpy(req.UserID, configInfo.UserId.c_str());
  strcpy(req.Password, configInfo.Password.c_str());

  int iResult = pTraderApi->ReqUserLogin(&req, ++iRequestID);
  BOOST_LOG_SEV(lg, info) << "--->>> 发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void FuturesTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
  BOOST_LOG_SEV(lg, info) << "--->>> " << "FuturesTraderSpi::OnRspUserLogin" << endl;
  if (bIsLast && !IsErrorRspInfo(pRspInfo))
  {
    // 保存会话参数
    FRONT_ID = pRspUserLogin->FrontID;
    SESSION_ID = pRspUserLogin->SessionID;
    ///获取当前交易日
    BOOST_LOG_SEV(lg, info) << "--->>> 获取当前交易日 = " << pTraderApi->GetTradingDay() << endl;
    ///投资者结算结果确认
    //ReqSettlementInfoConfirm();

    BOOST_LOG_SEV(lg, info) << "Current FrontID: " << FRONT_ID << " current Session ID: " << SESSION_ID;
  }
}


bool FuturesTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
  // 如果ErrorID != 0, 说明收到了错误的响应
  bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
  if (bResult)
    cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
  return bResult;
}

bool FuturesTraderSpi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
  return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
      (pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
      (pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}



void FuturesTraderSpi::ReqQryInstrument(const char *productIDName)
{
  struct CThostFtdcQryInstrumentField reqInstrument;
  memset(&reqInstrument, 0, sizeof(reqInstrument));
  if (productIDName != NULL)
  {
    strcpy(reqInstrument.ProductID, productIDName);
  }
  strcpy(reqInstrument.ExchangeID, "CFFEX");
  int ret = pTraderApi->ReqQryInstrument(&reqInstrument, ++iRequestID);
  while (UNDER_CTP_FLOW_CONTROL(ret)) {
    ret = pTraderApi->ReqQryInstrument(&reqInstrument, ++iRequestID);
  }
}

void FuturesTraderSpi::ReqQryInvestorPosition()
{
  CThostFtdcQryInvestorPositionField oreq;
  memset(&oreq, 0, sizeof(oreq));
  strcpy(oreq.BrokerID, configInfo.BrokerId.c_str());
  strcpy(oreq.InvestorID, configInfo.UserId.c_str());
  int ret = pTraderApi->ReqQryInvestorPosition(&oreq, ++iRequestID);
  while (UNDER_CTP_FLOW_CONTROL(ret))
  {
    ret = pTraderApi->ReqQryInvestorPosition(&oreq, ++iRequestID);
  }
}
