#ifndef _FUTURES_TRADING_CLIENT_
#define _FUTURES_TRADING_CLIENT_

#include "FuturesTraderSpi.h"
#include "FuturesMdSpi.h"
#include "FuturesDataStructures.h"
#include "FuturesUtil.h"


#ifdef SIMULATION
#define BASE_TRADING_CLIENT DummyFuturesTradingClient
#define RECORD_TABLE "dailyPositionRecordSimu"
#else
#define BASE_TRADING_CLIENT FuturesTradingClient
#define RECORD_TABLE "dailyPositionRecord"
#endif



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
	void initLogger() {
		stringstream ss;
		ss << getStrategyId() << "\\" << getStrategyId() << "_%Y%m%d_%N.log";
		::initLogger(ss.str());
	}

	virtual string getStrategyId() {
		return "Default";
	}
};


class FuturesTradingClient_EXPORTS FuturesTradingClientLimit : public FuturesMdSpi, public FuturesTraderSpi{
protected:
	map<string, CThostFtdcOrderField> orderRefToInfoMap;
	int size;
	int iRetValue = -1;
	string lastExchangeId;
	string lastOrderSysId;	

	bool insertOrderSubmitted = false;
	bool cancelOrderSubmitted = false;
	bool insertOrCancelSuccess = false;
	int numRtnOrderReceivedAfterInsertOrCancel = 0;

public:	
	string lastOrderRef;

	template<typename T>
	bool isMyOrder(T *pOrder); 

	bool isMyOrderOfCurrentSession(CThostFtdcOrderField *pOrder);

	void initLogger() {
		stringstream ss;
		ss << getStrategyId() << "\\" << getStrategyId() << "_%Y%m%d_%N.log";
		::initLogger(ss.str());
	}

	virtual string getStrategyId() {
		return "Default";
	}

	FuturesConfigInfo configInfo;

	FuturesTradingClientLimit(const FuturesConfigInfo& _configInfo) : configInfo(_configInfo), FuturesTraderSpi(_configInfo), FuturesMdSpi(_configInfo), size(_configInfo.Size){
		BOOST_LOG_SEV(lg, info) << "--->>> " << "FuturesTradingClient::FuturesTradingClient " << configInfo.TraderFrontAddr << " " << configInfo.MdFrontAddr << " size: " << size << endl;
	}

	bool InsertOrder(const string &sContract, double fPrice, int nVolume, ORDER_DIRECTION nDirection);

	void CancelOrder(const string& orderRef);

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
		lock1.lock();
		OnRtnDepthMarketDataShell(p);
		lock1.unlock();
	}

	virtual int OnRtnDepthMarketDataShell(CThostFtdcDepthMarketDataField * p) { 
		return -1; 
	}

	void ReqQryInvestorPosition();

	void initTradingClient();

	virtual void statusSettingOnOrderAccepted(CThostFtdcOrderField *p) {}
	virtual void statusSettingOnOrderTraded(CThostFtdcOrderField *p) {}
	virtual void statusSettingOnOrderCanceled(CThostFtdcOrderField *p) {}

	virtual void statusSettingOnOrderInsertionFailed(CThostFtdcOrderField *p) {}

	// Because OnRspOrderAction() and OnErrRtnOrderAction() have different first paramter, we cannot unify them
	virtual void statusSettingOnOrderCancelFailed(const string& orderRef) {}

};



#endif