#ifndef MARKETDATASPI_H
#define MARKETDATASPI_H


#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <FuturesDataStructures.h>

using namespace std;

#include "securityCTP/SecurityFtdcMdApi.h"


class SecurityMdSpi : public CSecurityFtdcMdSpi
{
	std::mutex mtx2;
	std::unique_lock<mutex> lock2;
	std::condition_variable cv2;
	
	SecurityCtpConfigInfo configInfo;
	SecurityMdSpi(const SecurityCtpConfigInfo& _configInfo);

public:
	SecurityMdSpi(CSecurityFtdcMdApi *pUserApi);
	void init();

	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();
	///登录请求响应
	virtual void OnRspUserLogin(CSecurityFtdcRspUserLoginField *pRspUserLogin, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///深度行情通知
	virtual void OnRtnDepthMarketData(CSecurityFtdcDepthMarketDataField *md);
	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	virtual void OnFrontDisconnected(int nReason);

	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	///@param nTimeLapse 距离上次接收报文的时间
	virtual void OnHeartBeatWarning(int nTimeLapse);

	///错误应答
	virtual void OnRspError(CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void SubscribeMarketData(const vector<string>& instruments, int nCount);

	void SubscribeMarketData(const string& instrument);

private:
	
	CSecurityFtdcMdApi *pMdApi;
};

#endif // MARKETDATASPI_H
