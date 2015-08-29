#ifndef _FUTURES_TRADING_CLIENT_
#define _FUTURES_TRADING_CLIENT_

#include "FuturesTraderSpi.h"
#include "FuturesMdSpi.h"
#include "FuturesDataStructures.h"
#include "FuturesUtil.h"

class DummyFuturesTradingClient {
protected:
	int size = 1;
	int iRetValue = -1;
	int iRequestID = 0;
	src::severity_logger< severity_level > lg;
public:
	DummyFuturesTradingClient(const FuturesConfigInfo& configInfo):size(configInfo.Size) {}
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


class FuturesTradingClient_EXPORTS FuturesTradingClient : public FuturesMdSpi, public FuturesTraderSpi{
protected:
	map<string, CThostFtdcOrderField> orderRefToInfoMap;
	int size = 1;
	int iRetValue = -1;
	string lastExchangeId;
	string lastOrderSysId;	
public:	
	string lastOrderRef;
	
	template<typename T>
	bool isMyOrder(T *pOrder);

	bool isMyOrderOfCurrentSession(CThostFtdcOrderField *pOrder);

	FuturesConfigInfo configInfo;

	FuturesTradingClient(const FuturesConfigInfo& _configInfo) : configInfo(_configInfo), FuturesTraderSpi(_configInfo), FuturesMdSpi(_configInfo), size(_configInfo.Size){
		cerr << "--->>> " << "FuturesTradingClient::FuturesTradingClient " << configInfo.TraderFrontAddr << " " << configInfo.MdFrontAddr << " size: " << size << endl;
	}

	void InsertOrder(const string &sContract, double fPrice, int nVolume, ORDER_DIRECTION nDirection);

	void CancelOrder(const string& orderRef);

	void ModifyOrder(const string& orderRef, double newPrice);

	///投资者结算结果确认响应
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	}

	///请求查询合约响应
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	}

	///请求查询资金账户响应
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	}

	///请求查询投资者持仓响应
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	}

	virtual void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	}

	
	///错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);	
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
	virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);



	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *p) {
		OnRtnDepthMarketDataShell(p);
	}

	virtual int OnRtnDepthMarketDataShell(CThostFtdcDepthMarketDataField * p) { 
		return -1; 
	}

	void ReqQryInvestorPosition();

	void initTradingClient();

};



#endif