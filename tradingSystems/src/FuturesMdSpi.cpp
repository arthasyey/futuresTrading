#include "../include/FuturesMdSpi.h"


FuturesMdSpi::FuturesMdSpi(const FuturesConfigInfo& _configInfo) : configInfo(_configInfo), lock2(mtx2, defer_lock) {}

void FuturesMdSpi::OnFrontConnected() {
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy_s(req.BrokerID, configInfo.MdBrokerId.c_str());
	strcpy_s(req.UserID, configInfo.MdUserId.c_str());
	strcpy_s(req.Password, configInfo.MdPassword.c_str());
	int iResult = pMdApi->ReqUserLogin(&req, 0);
	cerr << "--->>> 发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}


void FuturesMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	cerr << "--->>> " << "FuturesMdSpi::OnRspUserLogin" << endl;
	if (bIsLast)
	{
		std::unique_lock<mutex> localLock(mtx2);
		cv2.notify_all();
		localLock.unlock();
		SubscribeMarketData(configInfo.Contract);
	}	
}


int FuturesMdSpi::SubscribeMarketData(const vector<string>& instruments, int nCount) {
	int count = instruments.size();
	char** pInstruments = new char*[count];
	for (int i = 0; i < count; ++i)
	{
		pInstruments[i] = new char[20];
		strcpy_s(pInstruments[i], 20, instruments[i].c_str());
	}

	int ret = pMdApi->SubscribeMarketData(pInstruments, instruments.size());
	for (int i = 0; i < count; ++i)
		delete pInstruments[i];
	delete pInstruments;

	return ret;
}


int FuturesMdSpi::SubscribeMarketData(const string& instrument) {
	vector<string> instruments(1, instrument);
	return SubscribeMarketData(instruments, 1);
}
