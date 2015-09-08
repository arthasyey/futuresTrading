#ifndef _FUTURES_TRADING_CLIENT_
#define _FUTURES_TRADING_CLIENT_

#include <FuturesUtil.h>
#include <zhelpers.hpp>
#include <fstream>
#include <map>

enum LaunchMode {
  SIM,
  PROD
};


template<LaunchMode mode>
class FuturesTradeClient {
private:
  zmq::context_t context;
  zmq::socket_t requestPusher;
  zmq::socket_t responseSubscriber;
  zmq::socket_t feedSubscriber;

protected:
  string symbol;

  int longOpenPositions = 0;
  int shortOpenPositions = 0;
  string closePositionOrderRef;
  string openPositionOrderRef;
  int remainingPositionsToBeOpened = 0;
  int remainingPositionsToBeClosed = 0;

  map<string, CThostFtdcOrderField> orderRefToInfoMap;
  int size;
  FuturesConfigInfo configInfo;

  //string lastExchangeId;
  //string lastOrderSysId;

  bool insertOrderSubmitted = false;
  bool insertOrderAccepted = false;
  bool cancelOrderSubmitted = false;
  int numRtnOrderReceivedAfterInsertOrCancel = 0;

  enum InsertOrderRequirement {
    CancelIfExpired,
    ForceComplete
  };


  //  double lastOrderPriceSlippage;
  //  double lastOrderBasisPrice;
  //  string lastOrderSubmitTime;
  //  int lastOrderWaitTimeSecs;
  //  string lastOrderStopWaitTime;


  //CThostFtdcInputOrderField lastOrderRequest;
  //int lastOrderSize;
  //ORDER_DIRECTION lastOrderDirection;

  CThostFtdcDepthMarketDataField lastTick;

public:
  FuturesTradeClient(const FuturesConfigInfo& _configInfo);
  string lastOrderRef;
  int iRequestID = 0;

  template<typename T>
  bool isMyOrder(T *pOrder);

  bool isMyOrderOfCurrentSession(CThostFtdcOrderField *pOrder);



  void insertOrder(const string &symbol, double fPrice, int nVolume, ORDER_DIRECTION nDirection);
  void cancelOrder(const string& orderRef);
  bool IsTradingOrder(CThostFtdcOrderField *pOrder);

  void run();

  virtual void onFeed(CThostFtdcDepthMarketDataField *pFeed) {}
  virtual void onOrderAck(CThostFtdcOrderField *pOrder);

  virtual void onOrderFilled(CThostFtdcTradeField *pTrade);
  virtual void onCancelFilled(CThostFtdcOrderField *pOrder);

  virtual void onOrderRejected(RejectInfo *pInfo);
  virtual void onCancelRejected(RejectInfo *pInfo);

  void initLogger() {
    stringstream ss;
    ss << getStrategyId() << "\\" << getStrategyId() << "_%Y%m%d_%N.log";
    ::initLogger(ss.str());
  }

  virtual string getStrategyId() = 0;

  virtual ~FuturesTradeClient() {}
};


template<LaunchMode mode>
FuturesTradeClient<mode>::FuturesTradeClient(const FuturesConfigInfo& _configInfo): context(), requestPusher(context, ZMQ_PUSH),
responseSubscriber(context, ZMQ_SUB), feedSubscriber(context, ZMQ_SUB), size(_configInfo.size), configInfo(_configInfo) {
  memset(&lastTick, 0, sizeof(CThostFtdcDepthMarketDataField));
  LOG_BOOST << configInfo.TraderFrontAddr << " " << configInfo.MdFrontAddr << " size: " << size << endl;
}

template<>
FuturesTradeClient<SIM>::FuturesTradeClient(const FuturesConfigInfo& _configInfo) : context(), requestPusher(context, ZMQ_PUSH),
responseSubscriber(context, ZMQ_SUB), feedSubscriber(context, ZMQ_SUB), symbol(_configInfo.Symbol), size(_configInfo.size), configInfo(_configInfo) {
  LOG_BOOST << configInfo.TraderFrontAddr << " " << configInfo.MdFrontAddr << " size: " << size << endl;
}

template<>
void FuturesTradeClient<SIM>::run() {
  string filePath = DATA_ROOT + "/" + configInfo.Symbol + "/" + configInfo.simDate + ".csv";
  ifstream ifs(filePath.c_str());
  string line;
  ifs >> line;
  int totalVolume = 0;
  double totalTurnOver = 0;
  cout << filePath << endl;
  while (ifs >> line) {
      CThostFtdcDepthMarketDataField feed;
      memset(&feed, 0, sizeof(CThostFtdcDepthMarketDataField));
      vector<string> segments = FuturesUtil::split(line, ',');
      string &updateTime = segments[0];
      int updateMillis = atoi(segments[1].c_str());
      double lastPrice = atof(segments[2].c_str());
      double avgPrice = atof(segments[3].c_str());
      double bidPrice1 = atof(segments[4].c_str());
      double askPrice1 = atof(segments[5].c_str());
      int bidVolume1 = atoi(segments[6].c_str());
      int askVolume1 = atoi(segments[7].c_str());
      int volume = atoi(segments[8].c_str());

      strcpy(feed.InstrumentID, symbol.c_str());
      strcpy(feed.UpdateTime, updateTime.c_str());
      feed.UpdateMillisec = updateMillis;
      feed.LastPrice = lastPrice;
      feed.BidPrice1 = bidPrice1;
      feed.AskPrice1 = askPrice1;
      feed.BidVolume1 = bidVolume1;
      feed.AskVolume1 = askVolume1;
      totalVolume += volume;
      totalTurnOver += volume * avgPrice;
      feed.Turnover = totalTurnOver;
      feed.Volume = totalVolume;

      lastTick = feed;
      this->onFeed(&feed);
  }
}

template<>
void FuturesTradeClient<SIM>::insertOrder(const string &sContract, double fPrice, int nVolume, ORDER_DIRECTION nDirection) {
  string orderRef = toString(++iRequestID);
  CThostFtdcTradeField trade;
  trade.Volume = 1;
  trade.Price = (nDirection == OPEN_LONG || nDirection == CLOSE_SHORT) ? lastTick.AskPrice1 : lastTick.BidPrice1;

  if (nDirection == OPEN_LONG || nDirection == OPEN_SHORT) {
      openPositionOrderRef = orderRef;
      strcpy(trade.OrderRef, orderRef.c_str());
  }
  else {
      closePositionOrderRef = orderRef;
      strcpy(trade.OrderRef, orderRef.c_str());
  }
  onOrderFilled(&trade);
}


template<LaunchMode mode>
void FuturesTradeClient<mode>::run() {
  requestPusher.connect(configInfo.tradeHandlerAddr.c_str());
  responseSubscriber.connect(configInfo.traderHandlerListen.c_str());
  feedSubscriber.connect(configInfo.feedHandlerAdddr.c_str());

  responseSubscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
  feedSubscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);

  zmq::pollitem_t items [] = {
      { responseSubscriber, 0, ZMQ_POLLIN, 0 },
      { feedSubscriber, 0, ZMQ_POLLIN, 0 }
  };

  while (true) {
      zmq::message_t response;
      zmq::poll (&items [0], 2, -1);

      if (items [0].revents & ZMQ_POLLIN) {
	  feedSubscriber.recv(&response);
	  char* data = (char*)response.data();
	  CThostFtdcDepthMarketDataField *pData = (CThostFtdcDepthMarketDataField*)(data + 1);
	  lastTick = *pData;
	  onFeed(pData);
      }
      if (items [1].revents & ZMQ_POLLIN) {
	  responseSubscriber.recv(&response);
	  assert(response.size() >= sizeof(MSG_TYPE));

	  char* data = (char*)response.data();
	  MSG_TYPE msgType = *((MSG_TYPE*)data);
	  switch (msgType)
	  {
	    case REP_RTN_TRADE: {
	      assert(response.size() == sizeof(MSG_TYPE)+sizeof(CThostFtdcTradeField));
	      data += sizeof(MSG_TYPE);
	      CThostFtdcTradeField *pTradeField = (CThostFtdcTradeField *)data;
	      onOrderFilled(pTradeField);
	      break;
	    }
	    case REP_RTN_ORDER: {
	      assert(response.size() == sizeof(MSG_TYPE)+sizeof(CThostFtdcOrderField));
	      data += sizeof(MSG_TYPE);
	      CThostFtdcOrderField *pOrderField = (CThostFtdcOrderField *)data;
	      if (pOrderField->OrderStatus == THOST_FTDC_OST_Canceled)
		{
		  onCancelFilled(pOrderField);
		}
	      else {
		  onOrderAck(pOrderField);
	      }
	      break;
	    }
	    case REP_RSP_ORDER_INSERT:
	    case REP_ERR_RTN_ORDER_INSERT:{
	      assert(response.size() == sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderField)+sizeof(CThostFtdcRspInfoField)+sizeof(int)* 2 ||
		     response.size() == sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderField)+sizeof(CThostFtdcRspInfoField));
	      data += sizeof(MSG_TYPE);
	      CThostFtdcInputOrderField *pInputOrderField = (CThostFtdcInputOrderField *)data;

	      data += sizeof(CThostFtdcInputOrderField);
	      CThostFtdcRspInfoField *pRspInfo = (CThostFtdcRspInfoField *)data;

	      RejectInfo info(pInputOrderField->OrderRef, pRspInfo->ErrorMsg);
	      onOrderRejected(&info);
	      break;
	    }

	    case REP_RSP_ORDER_ACTION: {
	      assert(response.size() == sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderActionField)+sizeof(CThostFtdcRspInfoField)+sizeof(int)* 2);
	      data += sizeof(MSG_TYPE);
	      CThostFtdcInputOrderActionField *pInputOrderActionField = (CThostFtdcInputOrderActionField *)data;

	      data += sizeof(CThostFtdcInputOrderActionField);
	      CThostFtdcRspInfoField *pRspInfo = (CThostFtdcRspInfoField *)data;

	      RejectInfo info(pInputOrderActionField->OrderRef, pRspInfo->ErrorMsg);
	      onOrderRejected(&info);
	      break;
	    }

	    case REP_ERR_RTN_ORDER_ACTION: {
	      assert(response.size() == sizeof(MSG_TYPE)+sizeof(CThostFtdcOrderActionField)+sizeof(CThostFtdcRspInfoField));
	      data += sizeof(MSG_TYPE);
	      CThostFtdcOrderActionField *pOrderActionField = (CThostFtdcOrderActionField *)data;

	      data += sizeof(CThostFtdcOrderActionField);
	      CThostFtdcRspInfoField *pRspInfo = (CThostFtdcRspInfoField *)data;

	      RejectInfo info(pOrderActionField->OrderRef, pRspInfo->ErrorMsg);
	      onOrderRejected(&info);
	      break;
	    }

	    default:{
	      LOG_BOOST << " response type:" << msgType << " this should not happen!";
	      break;
	    }
	  }
      }
  }
}

template<LaunchMode mode>
void FuturesTradeClient<mode>::cancelOrder(const string& orderRef) {
  CThostFtdcInputOrderActionField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, configInfo.BrokerId.c_str());
  strcpy(req.InvestorID, configInfo.UserId.c_str());
  strcpy(req.UserID, configInfo.UserId.c_str());

  if (orderRefToInfoMap.find(orderRef) == orderRefToInfoMap.end())
    {
      LOG_BOOST << "CancelOrder-- Order ref " << orderRef << " is not in the system";
      return;
    }

  if (!IsTradingOrder(&orderRefToInfoMap[orderRef]))
    {
      LOG_BOOST << "CancelOrder-- Order ref " << orderRef << FuturesUtil::futuresOrderFieldToString(&orderRefToInfoMap[orderRef]) << " is not trading order";
      return;
    }

  //assert(orderRefToInfoMap.find(orderRef) != orderRefToInfoMap.end() && "Order ref not in the info map");
  CThostFtdcOrderField &orderInfo = orderRefToInfoMap[orderRef];

  req.ActionFlag = THOST_FTDC_AF_Delete;
  req.FrontID = orderInfo.FrontID;
  req.SessionID = orderInfo.SessionID;
  strcpy(req.OrderRef, orderRef.c_str());
  strcpy(req.InstrumentID, orderInfo.InstrumentID);
  strcpy(req.ExchangeID, orderInfo.ExchangeID);
  strcpy(req.OrderSysID, orderInfo.OrderSysID);

  zmq::message_t request(sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderActionField));
  char* data = (char*)request.data();
  *((MSG_TYPE*)data) = REQ_ORDER_ACTION;
  data += sizeof(MSG_TYPE);
  memcpy(data, &req, sizeof(CThostFtdcInputOrderActionField));
  requestPusher.send(request);

  numRtnOrderReceivedAfterInsertOrCancel = 0;
  cancelOrderSubmitted = true;

  LOG_BOOST << " Submit cancel request:" << FuturesUtil::futuresInputOrderActionFieldToString(&req);
}


template<LaunchMode mode>
bool FuturesTradeClient<mode>::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
  return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
      (pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
      (pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}

template<LaunchMode mode>
void FuturesTradeClient<mode>::insertOrder(const string &sContract, double fPrice, int nVolume, ORDER_DIRECTION nDirection) {
  CThostFtdcInputOrderField req;
  memset(&req, 0, sizeof(req));

  stringstream ss;
  ss << getStrategyId();
  string iRequestIdStr = toString(iRequestID++);
  if (iRequestIdStr.length() <= 4) {
      ss << string(4 - iRequestIdStr.length(), '0');
      ss << iRequestIdStr;
  }
  else {
      ss << iRequestIdStr.substr(0, 4);
  }
  string orderRef = ss.str();
  strcpy(req.OrderRef, orderRef.c_str());

  strcpy(req.BrokerID, configInfo.BrokerId.c_str());
  strcpy(req.InvestorID, configInfo.UserId.c_str());
  strcpy(req.InstrumentID, sContract.c_str());
  if (fPrice == 0.0)
    {
      req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
      req.LimitPrice = 0.0;
      req.TimeCondition = THOST_FTDC_TC_IOC;
    }
  else
    {
      req.TimeCondition = THOST_FTDC_TC_GFD;
      req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
      req.LimitPrice = fPrice;
    }

  switch (nDirection)
  {
    case OPEN_LONG:
      req.Direction = THOST_FTDC_D_Buy;
      req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
      openPositionOrderRef = orderRef;
      break;
    case OPEN_SHORT:
      req.Direction = THOST_FTDC_D_Sell;
      req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
      openPositionOrderRef = orderRef;
      break;
    case CLOSE_LONG:
      req.Direction = THOST_FTDC_D_Sell;
      req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
      closePositionOrderRef = orderRef;
      break;
    case CLOSE_SHORT:
      req.Direction = THOST_FTDC_D_Buy;
      req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
      closePositionOrderRef = orderRef;
      break;
    default:
      break;
  }

  req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;

  req.VolumeTotalOriginal = nVolume;

  req.VolumeCondition = THOST_FTDC_VC_AV;
  req.MinVolume = 1;
  req.ContingentCondition = THOST_FTDC_CC_Immediately;
  req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
  req.IsAutoSuspend = 0;
  req.UserForceClose = 0;

  zmq::message_t request(sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderField));
  char* data = (char*)request.data();
  *((MSG_TYPE*)data) = REQ_ORDER_INSERT;
  data += sizeof(MSG_TYPE);
  memcpy(data, &req, sizeof(CThostFtdcInputOrderField));

  requestPusher.send(request);

  LOG_BOOST << " Submit insert order request:" << FuturesUtil::futuresInputOrderFieldToString(&req);
}

template<>
template<typename T>
bool FuturesTradeClient<SIM>::isMyOrder(T *pOrder) {
  return true;
}

template<>
template<typename T>
bool FuturesTradeClient<PROD>::isMyOrder(T *pOrder) {
  string orderRef = pOrder->OrderRef;
  bool ret = orderRef.length() > 7 && orderRef.substr(orderRef.length() - 7, 3) == getStrategyId();
  LOG_BOOST << pOrder->OrderRef << " is my order:" << ret;
  return ret;
}

template<LaunchMode mode>
void FuturesTradeClient<mode>::onOrderAck(CThostFtdcOrderField *pOrder) {
  LOG_BOOST << FuturesUtil::futuresOrderFieldToString(pOrder);
}

template<LaunchMode mode>
void FuturesTradeClient<mode>::onOrderFilled(CThostFtdcTradeField *pTrade) {
  if (strcpy(pTrade->OrderRef, openPositionOrderRef.c_str()) == 0) {
      if (pTrade->Direction == THOST_FTDC_D_Buy){
	  longOpenPositions += pTrade->Volume;
      }
      else {
	  shortOpenPositions += pTrade->Volume;
      }
      remainingPositionsToBeOpened -= pTrade->Volume;
  }
  else {
      if (pTrade->Direction == THOST_FTDC_D_Buy) {
	  shortOpenPositions -= pTrade->Volume;
      }
      else {
	  longOpenPositions -= pTrade->Volume;
      }
      remainingPositionsToBeClosed -= pTrade->Volume;
  }
  LOG_BOOST << FuturesUtil::futuresTradeFieldToString(pTrade);
}

template<LaunchMode mode>
void FuturesTradeClient<mode>::onOrderRejected(RejectInfo *pInfo) {
  if (pInfo->orderRef == openPositionOrderRef) {
      remainingPositionsToBeOpened = 0;
  }
  else {
      remainingPositionsToBeClosed = 0;
  }
  LOG_BOOST << FuturesUtil::rejectInfoToString(pInfo);
}

template<LaunchMode mode>
void FuturesTradeClient<mode>::onCancelFilled(CThostFtdcOrderField *pOrder) {
  if (strcpy(pOrder->OrderRef, openPositionOrderRef.c_str()) == 0) {
      remainingPositionsToBeOpened = 0;
  }
  else {
      remainingPositionsToBeClosed = 0;
  }
  LOG_BOOST << FuturesUtil::futuresOrderFieldToString(pOrder);
}

template<LaunchMode mode>
void FuturesTradeClient<mode>::onCancelRejected(RejectInfo *pInfo) {
  LOG_BOOST << FuturesUtil::rejectInfoToString(pInfo);
}

#endif
