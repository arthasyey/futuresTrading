#ifndef _DUMMY_FUTURES_TRADING_CLIENT_
#define _DUMMY_FUTURES_TRADING_CLIENT_

#include "FuturesDataStructures.h"

class DummyFuturesTradingClient {
protected:
	int size = 1;
	int iRetValue = -1;
	int iRequestID = 0;
	src::severity_logger< severity_level > lg;
public:
	DummyFuturesTradingClient(const FuturesConfigInfo& configInfo) :size(configInfo.Size) {}
	void InsertOrder(const string &sContract, double fPrice, int nVolume, ORDER_DIRECTION nDirection) {}
	virtual int OnRtnDepthMarketDataShell(CThostFtdcDepthMarketDataField * p) { return -1; }
	void initTradingClient(){}

	bool isMyOrder(CThostFtdcOrderField *pOrder) {
		return true;
	}
	bool isMyOrderOfCurrentSession(CThostFtdcOrderField *pOrder) {
		return true;
	}
	void CancelOrder(const string& orderRef) {

	}
	virtual string getStrategyId() {
		return "Default";
	}
};

#endif