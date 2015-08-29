#ifndef _FUTURES_TRADER_SPI_
#define _FUTURES_TRADER_SPI_

#include <mutex>
#include <condition_variable>

#include "FuturesDataStructures.h"
#include "../ctp/ThostFtdcTraderApi.h"
#include "../include/logging.h"


extern src::severity_logger< severity_level > lg;


class FuturesTraderSpi : public CThostFtdcTraderSpi {
protected:	
	std::mutex mtx1;
	std::unique_lock<mutex> lock1;
	std::condition_variable cv1;

	FuturesConfigInfo configInfo;

	// 会话参数
	TThostFtdcFrontIDType	FRONT_ID;	//前置编号
	TThostFtdcSessionIDType	SESSION_ID;	//会话编号


public:
	CThostFtdcTraderApi* pTraderApi;

	// 请求编号
	int iRequestID;

	FuturesTraderSpi(const FuturesConfigInfo& _configInfo);

	virtual void OnFrontConnected();

	///登录请求响应
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///投资者结算结果确认响应
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

	///请求查询合约响应
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

	///请求查询资金账户响应
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

	///请求查询投资者持仓响应
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

	virtual void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

	///报单录入请求响应
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

	///报单操作请求响应
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

	///错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){}

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	void OnFrontDisconnected(int nReason) {}

	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	virtual void OnHeartBeatWarning(int nTimeLapse){}

	///报单通知
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder) {}

	///成交通知
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade){}


	// 是否收到成功的响应
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);

	// 是否我的报单回报
	bool IsMyOrder(CThostFtdcOrderField *pOrder);
	// 是否正在交易的报单
	bool IsTradingOrder(CThostFtdcOrderField *pOrder);

	void ReqQryInvestorPosition();

	void ReqQryInstrument(const char *productIDName = NULL);

	void initLogger() {
		stringstream ss;
		ss << getStrategyId() << "\\" << getStrategyId() << "_%Y%m%d_%N.log";
		::initLogger(ss.str());
	}

	virtual string getStrategyId() {
		return "Default";
	}
};




#endif // !_FUTURES_TRADER_SPI_
