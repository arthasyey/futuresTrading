#ifndef _FUTURES_MD_SPI_
#define _FUTURES_MD_SPI_

#include <mutex>
#include <condition_variable>

#include "../CTPLib/ThostTraderApi/ThostFtdcMdApi.h"
#include "FuturesDataStructures.h"


class FuturesTradingClient_EXPORTS FuturesMdSpi : public CThostFtdcMdSpi {
protected:	
	std::mutex mtx2;
	std::unique_lock<mutex> lock2;
	std::condition_variable cv2;

	FuturesConfigInfo configInfo;

	FuturesMdSpi(const FuturesConfigInfo& _configInfo);


public:
	CThostFtdcMdApi* pMdApi;

	virtual void OnFrontConnected();

	virtual void OnFrontDisconnected(int nReason){};

	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	///@param nTimeLapse 距离上次接收报文的时间
	virtual void OnHeartBeatWarning(int nTimeLapse){};


	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);// {}

	///登出请求响应
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

	///错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

	///订阅行情应答
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		//cerr << "--->>> " << "FuturesMdSpi::OnRspSubMarketData: " << pSpecificInstrument->InstrumentID << endl;
	}

	///取消订阅行情应答
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

	///深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {}

	int SubscribeMarketData(const vector<string>& instruments, int nCount);

	int SubscribeMarketData(const string& instrument);
};




#endif // !_TRADER_SPI_
