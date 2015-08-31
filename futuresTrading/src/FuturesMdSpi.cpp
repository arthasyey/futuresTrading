#include "../include/FuturesMdSpi.h"


FuturesMdSpi::FuturesMdSpi(const FuturesConfigInfo& _configInfo) : configInfo(_configInfo), lock2(mtx2, defer_lock) {
	pMdApi = CThostFtdcMdApi::CreateFtdcMdApi();
	pMdApi->RegisterSpi(this);

	pMdApi->RegisterFront(const_cast<char*>(FuturesMdSpi::configInfo.MdFrontAddr.c_str()));
	pMdApi->Init();
}

void FuturesMdSpi::OnFrontConnected() {
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, configInfo.MdBrokerId.c_str());
	strcpy(req.UserID, configInfo.MdUserId.c_str());
	strcpy(req.Password, configInfo.MdPassword.c_str());
	pMdApi->ReqUserLogin(&req, 0);
	cerr << "--->>> Connect to Md Frontend: " << ((iResult == 0) ? "Succeed" : "Failure") << endl;
}


void FuturesMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	cerr << "--->>> " << __FUNCTION__ << endl;
	if (bIsLast)
	{
		std::unique_lock<mutex> localLock(mtx2);
		cv2.notify_all();
		localLock.unlock();
		SubscribeMarketData(configInfo.Contract);
	}	
}


int FuturesMdSpi::SubscribeMarketData(const vector<string>& instruments) {
	int count = instruments.size();
	char** pInstruments = new char*[count];
	for (int i = 0; i < instrument.size(); ++i)
	{
		pInstruments[i] = new char[20];
		strcpy(pInstruments[i], 20, instruments[i].c_str());
	}

	int ret = pMdApi->SubscribeMarketData(pInstruments, instruments.size());
	for (int i = 0; i < count; ++i)
		delete pInstruments[i];
	delete pInstruments;

	return ret;
}


int FuturesMdSpi::SubscribeMarketData(const string& instrument) {
	vector<string> instruments(1, instrument);
	return SubscribeMarketData(instruments);
}
