#ifndef _FUTURES_TRADING_SERVER_
#define _FUTURES_TRADING_SERVER_

#include "../include/FuturesTraderSpi.h"
#include "../include/FuturesDataStructures.h"
#include "../include/FuturesUtil.h"
#include "../include/zmq/zhelpers.hpp"


class FuturesTradingServer : public FuturesTraderSpi {
private:
	zmq::context_t context;
	zmq::socket_t requestPuller;
	zmq::socket_t responsePublisher;

public:
	FuturesConfigInfo configInfo;

	FuturesTradingServer(const FuturesConfigInfo& _configInfo) : configInfo(_configInfo), FuturesTraderSpi(_configInfo), 
		requestPuller(context, ZMQ_PULL), responsePublisher(context, ZMQ_PUB) {}


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


	void initTradingServer();
	void run();	
};



#endif
