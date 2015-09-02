#ifndef _FUTURES_TRADING_CLIENT_
#define _FUTURES_TRADING_CLIENT_

#include <FuturesUtil.h>
#include <zhelpers.hpp>
#include <logging.h>
#include <map>

enum LaunchMode {
  SIM,
  PROD
};


template<LaunchMode mode>
class FuturesTradeClient {
private:
  zmq::context_t context;
  zmq::socket_t feedSubscriber;
  zmq::socket_t requestPusher;
  zmq::socket_t responseSubscriber;

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
  int iRetValue = -1;
  string lastExchangeId;
  string lastOrderSysId;

  bool insertOrderSubmitted = false;
  bool insertOrderAccepted = false;
  bool cancelOrderSubmitted = false;
  int numRtnOrderReceivedAfterInsertOrCancel = 0;

  enum InsertOrderRequirement {
    CancelIfExpired,
    ForceComplete
  };


  InsertOrderRequirement lastOrderRequirement;
  double lastOrderPriceSlippage;
  double lastOrderBasisPrice;
  string lastOrderSubmitTime;
  int lastOrderWaitTimeSecs;
  string lastOrderStopWaitTime;


  CThostFtdcInputOrderField lastOrderRequest;
  int lastOrderSize;
  ORDER_DIRECTION lastOrderDirection;

  CThostFtdcDepthMarketDataField lastTick;

public:
  string lastOrderRef;
  int iRequestID = 0;

  template<typename T>
  bool isMyOrder(T *pOrder);

  bool isMyOrderOfCurrentSession(CThostFtdcOrderField *pOrder);

  FuturesConfigInfo configInfo;

  FuturesTradeClient(const FuturesConfigInfo& _configInfo);

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


#endif
