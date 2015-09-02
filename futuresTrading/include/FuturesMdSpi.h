#ifndef _FUTURES_MD_SPI_
#define _FUTURES_MD_SPI_

#include <mutex>
#include <condition_variable>

#include "../ctp/ThostFtdcMdApi.h"
#include "FuturesDataStructures.h"


class FeedHandler : public CThostFtdcMdSpi {
protected:	
	std::mutex mtx2;
	std::unique_lock<mutex> lock2;
	std::condition_variable cv2;

	FuturesConfigInfo config;

	FeedHandler(const FuturesConfigInfo& _configInfo);


public:
	CThostFtdcMdApi* pMdApi;

	virtual void OnFrontConnected();

	virtual void OnFrontDisconnected(int nReason){};

	///����ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	///@param nTimeLapse �����ϴν��ձ��ĵ�ʱ��
	virtual void OnHeartBeatWarning(int nTimeLapse){};


	///��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);// {}

	///�ǳ�������Ӧ
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

	///����Ӧ��
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

	///��������Ӧ��
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		//cerr << "--->>> " << "FuturesMdSpi::OnRspSubMarketData: " << pSpecificInstrument->InstrumentID << endl;
	}

	///ȡ��������Ӧ��
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

	///�������֪ͨ
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {}

	int SubscribeMarketData(const vector<string>& instruments, int nCount);

	int SubscribeMarketData(const string& instrument);
};




#endif // !_TRADER_SPI_
