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

	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();
	///��¼������Ӧ
	virtual void OnRspUserLogin(CSecurityFtdcRspUserLoginField *pRspUserLogin, CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///�������֪ͨ
	virtual void OnRtnDepthMarketData(CSecurityFtdcDepthMarketDataField *md);
	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	///@param nReason ����ԭ��
	///        0x1001 �����ʧ��
	///        0x1002 ����дʧ��
	///        0x2001 ����������ʱ
	///        0x2002 ��������ʧ��
	///        0x2003 �յ�������
	virtual void OnFrontDisconnected(int nReason);

	///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	///@param nTimeLapse �����ϴν��ձ��ĵ�ʱ��
	virtual void OnHeartBeatWarning(int nTimeLapse);

	///����Ӧ��
	virtual void OnRspError(CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void SubscribeMarketData(const vector<string>& instruments, int nCount);

	void SubscribeMarketData(const string& instrument);

private:
	
	CSecurityFtdcMdApi *pMdApi;
};

#endif // MARKETDATASPI_H
