#ifndef _FUTURES_TRADING_CLIENTNEW_
#define _FUTURES_TRADING_CLIENTNEW_

#include "FuturesMdSpi.h"
#include "FuturesDataStructures.h"
#include "FuturesUtil.h"
#include "zmq/zhelpers.hpp"
#include "logging.h"

extern src::severity_logger< severity_level > lg;


#ifdef SIMULATION
#define BASE_TRADING_CLIENT DummyFuturesTradingClient
#define RECORD_TABLE "dailyPositionRecordSimu"
#else
#define BASE_TRADING_CLIENT FuturesTradingClientNew
#define RECORD_TABLE "dailyPositionRecord"
#endif



class DummyFuturesTradingClient {
protected:
	int size = 1;
	int iRetValue = -1;
	src::severity_logger< severity_level > lg;
public:
	int iRequestID = 0;
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


class FuturesTradingClient_EXPORTS FuturesTradingClientNew : public FuturesMdSpi {
private:
	zmq::context_t context;
	zmq::socket_t requestPusher;
	zmq::socket_t responseSubscriber;

protected:
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

	CThostFtdcDepthMarketDataField curTick;
	CThostFtdcDepthMarketDataField lastTick;

public:	
	string lastOrderRef;
	int iRequestID = 0;

	template<typename T>
	bool isMyOrder(T *pOrder); 

	bool isMyOrderOfCurrentSession(CThostFtdcOrderField *pOrder);

	FuturesConfigInfo configInfo;

	FuturesTradingClientNew(const FuturesConfigInfo& _configInfo) : configInfo(_configInfo), FuturesMdSpi(_configInfo), size(_configInfo.Size),
		requestPusher(context, ZMQ_PUSH), responseSubscriber(context, ZMQ_SUB) {
		memset(&curTick, 0, sizeof(CThostFtdcDepthMarketDataField));
		memset(&lastTick, 0, sizeof(CThostFtdcDepthMarketDataField));
		BOOST_LOG_SEV(lg, info) << "--->>> " << "FuturesTradingClient::FuturesTradingClient " << configInfo.TraderFrontAddr << " " << configInfo.MdFrontAddr << " size: " << size << endl;
	}

	void InsertOrder(const string &sContract, double fPrice, int nVolume, ORDER_DIRECTION nDirection);

	void CancelOrder(const string& orderRef);

	void InsertOrder(const string &sContract, double fPrice, int nVolume, ORDER_DIRECTION nDirection, InsertOrderRequirement requirement, double priceSlippage, int waitTimeSecs);

	bool IsTradingOrder(CThostFtdcOrderField *pOrder);


	///Ͷ���߽�����ȷ����Ӧ
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	}

	///�����ѯ��Լ��Ӧ
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	}

	///�����ѯ�ʽ��˻���Ӧ
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	}

	///�����ѯͶ���ֲ߳���Ӧ
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	}

	virtual void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	}

	
	///����Ӧ��
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);	
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
	virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);



	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *p); 

	virtual int OnRtnDepthMarketDataShell(CThostFtdcDepthMarketDataField * p) { 
		return -1; 
	}

	void ReqQryInvestorPosition();

	void initTradingClient();

	virtual void statusSettingOnOrderAccepted(CThostFtdcOrderField *p) {}
	virtual void clientSettingOnOrderTraded(CThostFtdcOrderField *p) {}
	virtual void clientSettingOnOrderCanceled(CThostFtdcOrderField *p) {}

	virtual void clientSettingOnTradeComplete(CThostFtdcOrderField *p) {}

	virtual void clientSettingOnOrderInsertionFailed(CThostFtdcOrderField *p) {}

	// Because OnRspOrderAction() and OnErrRtnOrderAction() have different first paramter, we cannot unify them
	virtual void statusSettingOnOrderCancelFailed(const string& orderRef) {}

	void initLogger() {
		stringstream ss;
		ss << getStrategyId() << "\\" << getStrategyId() << "_%Y%m%d_%N.log";
		::initLogger(ss.str());
	}

	virtual string getStrategyId() = 0;
};



#endif