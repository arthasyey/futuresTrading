#include "../include/SecurityMdSpi.h"

#define EXCHANGE_SH_NAME    ("SSE")
#define EXCHANGE_SZ_NAME    ("SZE")

enum StockExchange {
	SSE,
	SZE
};

StockExchange getExchange(const string& code) { 
	string paddedCode = code;
	if (code.length() < 6)
	{
		string padding = string(6 - code.length(), '0');
		paddedCode = padding + code;
	}
	string prefix = paddedCode.substr(0, 2);
	if (prefix == "60" || prefix == "90" || code[0] == '7' || code[0] == '5')
		return SSE;
	else
		return SZE;
}


SecurityMdSpi::SecurityMdSpi(const SecurityCtpConfigInfo& _configInfo) : configInfo(_configInfo), lock2(mtx2, defer_lock) {}

void SecurityMdSpi::OnFrontConnected() {
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy_s(req.BrokerID, configInfo.MdBrokerId.c_str());
	strcpy_s(req.UserID, configInfo.MdUserId.c_str());
	strcpy_s(req.Password, configInfo.MdPassword.c_str());
	int iResult = pMdApi->ReqUserLogin(&req, 0);
	cerr << "--->>> 发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}


void SecurityMdSpi::OnRspUserLogin(CSecurityFtdcRspUserLoginField *pRspUserLogin, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	cerr << "--->>> " << "SecurityMdSpi::OnRspUserLogin" << endl;
	if (bIsLast)
	{
		std::unique_lock<mutex> localLock(mtx2);
		cv2.notify_all();
		localLock.unlock();
		SubscribeMarketData(configInfo.Contract);
	}
}


void SecurityMdSpi::SubscribeMarketData(const vector<string>& instruments, int nCount) {
	int count = instruments.size();
	char** pSSEInstruments = new char*[count];
	char** pSZEInstruments = new char*[count];

	int shInstrumentsCount = 0;
	int szInstrumentsCount = 0;

	for (int i = 0; i < count; ++i)
	{
		if (getExchange(instruments[i]) == SSE)
		{
			pSSEInstruments[shInstrumentsCount] = new char[20];
			strcpy_s(pSSEInstruments[shInstrumentsCount++], 20, instruments[i].c_str());
		}
		else {
			pSZEInstruments[szInstrumentsCount] = new char[20];
			strcpy_s(pSZEInstruments[szInstrumentsCount++], 20, instruments[i].c_str());
		}		
	}

	pMdApi->SubscribeMarketData(pSSEInstruments, shInstrumentsCount, EXCHANGE_SH_NAME);
	pMdApi->SubscribeMarketData(pSZEInstruments, szInstrumentsCount, EXCHANGE_SZ_NAME);

	for (int i = 0; i < shInstrumentsCount; ++i)
		delete pSSEInstruments[i];
	for (int i = 0; i < szInstrumentsCount; ++i)
		delete pSZEInstruments[i];

	delete pSSEInstruments;
	delete pSZEInstruments;	
}


void SecurityMdSpi::SubscribeMarketData(const string& instrument) {
	vector<string> instruments(1, instrument);
	return SubscribeMarketData(instruments, 1);
}
