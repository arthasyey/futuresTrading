#include "tradeHandler.h"
#include <cassert>

TradeHandler::TradeHandler(const FuturesConfigInfo& _configInfo) : configInfo(_configInfo), context(), requestPuller(context, ZMQ_PULL), responsePublisher(context, ZMQ_PUB) {
  BOOST_LOG_SEV(lg, info) << "Init trade handler";
  pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
  pTraderApi->RegisterSpi(this);
  pTraderApi->SubscribePublicTopic(THOST_TERT_RESUME);
  pTraderApi->SubscribePrivateTopic(THOST_TERT_RESUME);
  pTraderApi->RegisterFront(const_cast<char*>(TradeHandler::configInfo.TraderFrontAddr.c_str()));					// connect
  pTraderApi->Init();

  requestPuller.bind("ipc:///tmp/futuresTradeRequests");
  responsePublisher.bind("ipc:///tmp/futuresTradeResponses");
}

void TradeHandler::run() {
  while (true)
  {
    zmq::message_t req;
    requestPuller.recv(&req);
    char *data = (char*)req.data();
    MSG_TYPE msgType = *((MSG_TYPE*)data);
    data += sizeof(MSG_TYPE);
    switch (msgType)
    {
    case REQ_ORDER_INSERT: {
      CThostFtdcInputOrderField insertOrderRequest = *((CThostFtdcInputOrderField*)data);
      stringstream ss;
      ss << ++iRequestID;
      ss << insertOrderRequest.OrderRef;
      strcpy(insertOrderRequest.OrderRef, ss.str().c_str());
      pTraderApi->ReqOrderInsert(&insertOrderRequest, iRequestID);
      break;
    }
    case REQ_ORDER_ACTION: {
      CThostFtdcInputOrderActionField deleteOrderRequest = *((CThostFtdcInputOrderActionField*)data);
      pTraderApi->ReqOrderAction(&deleteOrderRequest, ++iRequestID);
      break;
    }
    default:
      BOOST_LOG_SEV(lg, info) << __FUNCTION__ << " req type: " << msgType << " This should not happen!";
    }
  }
}


void TradeHandler::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  if (pRspInfo->ErrorID != 0){
    BOOST_LOG_SEV(lg, info) << pRspInfo->ErrorMsg << endl;
  }
  else{
    BOOST_LOG_SEV(lg, info) << "--->>> OnRspError" << endl;
  }
}


void TradeHandler::OnRtnTrade(CThostFtdcTradeField *p) {
  zmq::message_t rep(sizeof(MSG_TYPE)+sizeof(CThostFtdcTradeField));
  *((MSG_TYPE*)rep.data()) = REP_RTN_TRADE;

  CThostFtdcTradeField *pTradeField = (CThostFtdcTradeField*)((char*)rep.data() + sizeof(MSG_TYPE));
  memcpy(pTradeField, p, sizeof(CThostFtdcTradeField));

  responsePublisher.send(rep);
  BOOST_LOG_SEV(lg, info) << __PRETTY_FUNCTION__ << FuturesUtil::futuresTradeFieldToString(p);
}

void TradeHandler::OnRspOrderAction(CThostFtdcInputOrderActionField *p, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  zmq::message_t rep(sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderActionField)+sizeof(CThostFtdcRspInfoField)+sizeof(int)+sizeof(int));
  *((MSG_TYPE*)rep.data()) = REP_RSP_ORDER_ACTION;

  CThostFtdcInputOrderActionField *pOrderInputField = (CThostFtdcInputOrderActionField*)((char*)rep.data() + sizeof(MSG_TYPE));
  memcpy(pOrderInputField, p, sizeof(CThostFtdcInputOrderActionField));

  CThostFtdcRspInfoField *pRspInfoField = (CThostFtdcRspInfoField*)((char*)rep.data() + sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderActionField));
  memcpy(pRspInfoField, pRspInfo, sizeof(CThostFtdcRspInfoField));

  int *pNRequestID = (int*)((char*)rep.data() + sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderActionField)+sizeof(CThostFtdcRspInfoField));
  *pNRequestID = nRequestID;

  int *pIsLast = (int*)((char*)rep.data() + sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderActionField)+sizeof(CThostFtdcRspInfoField)+sizeof(int));
  *pIsLast = bIsLast ? 1 : 0;

  responsePublisher.send(rep);

  BOOST_LOG_SEV(lg, info) << __PRETTY_FUNCTION__ << FuturesUtil::futuresInputOrderActionFieldToString(p) << FuturesUtil::futuresRspInfoToString(pRspInfo);
}

void TradeHandler::OnErrRtnOrderAction(CThostFtdcOrderActionField *p, CThostFtdcRspInfoField *pRspInfo) {
  if (!p)
    return;

  zmq::message_t rep(sizeof(MSG_TYPE)+sizeof(CThostFtdcOrderActionField)+sizeof(CThostFtdcRspInfoField));
  char* data = (char*)rep.data();

  *((MSG_TYPE*)data) = REP_ERR_RTN_ORDER_ACTION;
  data += sizeof(MSG_TYPE);

  CThostFtdcOrderActionField *pOrderActionField = (CThostFtdcOrderActionField*)(data);
  memcpy(pOrderActionField, p, sizeof(CThostFtdcOrderActionField));
  data += sizeof(CThostFtdcOrderActionField);

  CThostFtdcRspInfoField *pRspInfoField = (CThostFtdcRspInfoField*)(data);
  memcpy(pRspInfoField, pRspInfo, sizeof(CThostFtdcRspInfoField));

  responsePublisher.send(rep);

  BOOST_LOG_SEV(lg, info) << __PRETTY_FUNCTION__ << FuturesUtil::futuresOrderActionFieldToString(p) << FuturesUtil::futuresRspInfoToString(pRspInfo);
}

void TradeHandler::OnRtnOrder(CThostFtdcOrderField * p) {
  zmq::message_t rep(sizeof(MSG_TYPE)+sizeof(CThostFtdcOrderField));
  *((MSG_TYPE*)rep.data()) = REP_RTN_ORDER;

  CThostFtdcOrderField *pOrderField = (CThostFtdcOrderField*)((char*)rep.data() + sizeof(MSG_TYPE));
  memcpy(pOrderField, p, sizeof(CThostFtdcOrderField));
  responsePublisher.send(rep);
  BOOST_LOG_SEV(lg, info) << __PRETTY_FUNCTION__ << FuturesUtil::futuresOrderFieldToString(p);
}

void TradeHandler::OnErrRtnOrderInsert(CThostFtdcInputOrderField *p, CThostFtdcRspInfoField *pRspInfo) {
  zmq::message_t rep(sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderField)+sizeof(CThostFtdcRspInfoField));
  *((MSG_TYPE*)rep.data()) = REP_ERR_RTN_ORDER_INSERT;

  CThostFtdcInputOrderField *pOrderInputField = (CThostFtdcInputOrderField*)((char*)rep.data() + sizeof(MSG_TYPE));
  memcpy(pOrderInputField, p, sizeof(CThostFtdcInputOrderField));

  CThostFtdcRspInfoField *pRspInfoField = (CThostFtdcRspInfoField*)((char*)rep.data() + sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderField));
  memcpy(pRspInfoField, pRspInfo, sizeof(CThostFtdcRspInfoField));

  responsePublisher.send(rep);

  BOOST_LOG_SEV(lg, info) << __PRETTY_FUNCTION__ << FuturesUtil::futuresInputOrderFieldToString(p) << FuturesUtil::futuresRspInfoToString(pRspInfo);
}

void TradeHandler::OnRspOrderInsert(CThostFtdcInputOrderField *p, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  zmq::message_t rep(sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderField)+sizeof(CThostFtdcRspInfoField)+sizeof(int)+sizeof(int));
  *((MSG_TYPE*)rep.data()) = REP_RSP_ORDER_INSERT;

  CThostFtdcInputOrderField *pOrderInputField = (CThostFtdcInputOrderField*)((char*)rep.data() + sizeof(MSG_TYPE));
  memcpy(pOrderInputField, p, sizeof(CThostFtdcInputOrderField));

  CThostFtdcRspInfoField *pRspInfoField = (CThostFtdcRspInfoField*)((char*)rep.data() + sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderField));
  memcpy(pRspInfoField, pRspInfo, sizeof(CThostFtdcRspInfoField));

  int *pNRequestID = (int*)((char*)rep.data() + sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderField)+sizeof(CThostFtdcRspInfoField));
  *pNRequestID = nRequestID;

  int *pIsLast = (int*)((char*)rep.data() + sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderField)+sizeof(CThostFtdcRspInfoField)+sizeof(int));
  *pIsLast = bIsLast ? 1 : 0;

  responsePublisher.send(rep);

  LOG_BOOST  << FuturesUtil::futuresInputOrderFieldToString(p) << FuturesUtil::futuresRspInfoToString(pRspInfo);
}

void TradeHandler::OnFrontDisconnected(int nReason) {
  LOG_BOOST << nReason;
  pTraderApi->RegisterFront(const_cast<char*>(TradeHandler::configInfo.TraderFrontAddr.c_str()));
}

void TradeHandler::OnFrontConnected() {
  CThostFtdcReqUserLoginField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, configInfo.BrokerId.c_str());
  strcpy(req.UserID, configInfo.UserId.c_str());
  strcpy(req.Password, configInfo.Password.c_str());

  int iResult = pTraderApi->ReqUserLogin(&req, ++iRequestID);
  LOG_BOOST << ((iResult == 0) ? "succeed" : "failure");
}

void TradeHandler::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
  if (bIsLast && !IsErrorRspInfo(pRspInfo))
  {
    FRONT_ID = pRspUserLogin->FrontID;
    SESSION_ID = pRspUserLogin->SessionID;
    LOG_BOOST << "Current FrontID: " << FRONT_ID << " current Session ID: " << SESSION_ID;
  }
}


bool TradeHandler::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
  bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
  if (bResult)
    cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
  return bResult;
}

bool TradeHandler::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
  return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
      (pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
      (pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}


void TradeHandler::ReqQryInstrument(const char *productIDName)
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

void TradeHandler::ReqQryInvestorPosition()
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


