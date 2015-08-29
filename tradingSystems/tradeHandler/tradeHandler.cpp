#include "tradeHandler.h"
#include <cassert>


void FuturesTradingServer::initTradingServer() {
	//initLogger();
	BOOST_LOG_SEV(lg, info) << "Init trading client";
	pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
	pTraderApi->RegisterSpi(this);
	pTraderApi->SubscribePublicTopic(THOST_TERT_RESUME);												// 注册公有流
	pTraderApi->SubscribePrivateTopic(THOST_TERT_RESUME);												// 注册私有流

	lock1.lock();
	pTraderApi->RegisterFront(const_cast<char*>(FuturesTraderSpi::configInfo.TraderFrontAddr.c_str()));					// connect
	pTraderApi->Init();
	cv1.wait(lock1);
	lock1.unlock();

	requestPuller.bind("inproc//*:9995");
	responsePublisher.bind("inproc//*:9996");
}

void FuturesTradingServer::run() {	
	while (true)
	{
		zmq::message_t req;
		requestPuller.recv(&req);
		char *data = (char*)req.data();
		MSG_TYPE msgType = *((MSG_TYPE*)data);
		data += sizeof(MSG_TYPE);
		switch (msgType)
		{
		case REQ_ORDER_INSERT: {
								   CThostFtdcInputOrderField insertOrderRequest = *((CThostFtdcInputOrderField*)data);
								   stringstream ss;
								   ss << iRequestID++;
								   ss << insertOrderRequest.OrderRef;
								   strcpy(insertOrderRequest.OrderRef, ss.str().c_str());								   
								   pTraderApi->ReqOrderInsert(&insertOrderRequest, iRequestID);
								   break;
		}
		case REQ_ORDER_ACTION: {
								   CThostFtdcInputOrderActionField deleteOrderRequest = *((CThostFtdcInputOrderActionField*)data);
								   pTraderApi->ReqOrderAction(&deleteOrderRequest, iRequestID++);
								   break;
		}
		default:
			BOOST_LOG_SEV(lg, info) << __FUNCTION__ << " req type: " << msgType << " This should not happen!";
		}
	}
}


void FuturesTradingServer::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (pRspInfo->ErrorID != 0){
		BOOST_LOG_SEV(lg, info) << pRspInfo->ErrorMsg << endl;
	}
	else{
		BOOST_LOG_SEV(lg, info) << "--->>> OnRspError" << endl;
	}
}


void FuturesTradingServer::OnRtnTrade(CThostFtdcTradeField *p) {
	zmq::message_t rep(sizeof(MSG_TYPE)+sizeof(CThostFtdcTradeField));
	*((MSG_TYPE*)rep.data()) = REP_RTN_TRADE;

	CThostFtdcTradeField *pTradeField = (CThostFtdcTradeField*)((char*)rep.data() + sizeof(MSG_TYPE));
	memcpy(pTradeField, p, sizeof(CThostFtdcTradeField));

	responsePublisher.send(rep);
	BOOST_LOG_SEV(lg, info) << "OnRtnTrade--" << FuturesUtil::futuresTradeFieldToString(p);
}

void FuturesTradingServer::OnRspOrderAction(CThostFtdcInputOrderActionField *p, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	zmq::message_t rep(sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderActionField)+sizeof(CThostFtdcRspInfoField)+sizeof(int)+sizeof(int));
	*((MSG_TYPE*)rep.data()) = REP_RSP_ORDER_ACTION;

	CThostFtdcInputOrderActionField *pOrderInputField = (CThostFtdcInputOrderActionField*)((char*)rep.data() + sizeof(MSG_TYPE));
	memcpy(pOrderInputField, p, sizeof(CThostFtdcInputOrderActionField));

	CThostFtdcRspInfoField *pRspInfoField = (CThostFtdcRspInfoField*)((char*)rep.data() + sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderActionField));
	memcpy(pRspInfoField, pRspInfo, sizeof(CThostFtdcRspInfoField));

	int *pNRequestID = (int*)((char*)rep.data() + sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderActionField)+sizeof(CThostFtdcRspInfoField));
	*pNRequestID = nRequestID;

	int *pIsLast = (int*)((char*)rep.data() + sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderActionField)+sizeof(CThostFtdcRspInfoField)+sizeof(int));
	*pIsLast = bIsLast ? 1 : 0;

	responsePublisher.send(rep);
	
    BOOST_LOG_SEV(lg, info) << "OnRspOrderAction --" << FuturesUtil::futuresInputOrderActionFieldToString(p) << FuturesUtil::futuresRspInfoToString(pRspInfo);
}

void FuturesTradingServer::OnErrRtnOrderAction(CThostFtdcOrderActionField *p, CThostFtdcRspInfoField *pRspInfo) {
	if (!p)
		return;
	
	zmq::message_t rep(sizeof(MSG_TYPE)+sizeof(CThostFtdcOrderActionField)+sizeof(CThostFtdcRspInfoField));
	char* data = (char*)rep.data();
	
	*((MSG_TYPE*)data) = REP_ERR_RTN_ORDER_ACTION;
	data += sizeof(MSG_TYPE);
	
	CThostFtdcOrderActionField *pOrderActionField = (CThostFtdcOrderActionField*)(data);
	memcpy(pOrderActionField, p, sizeof(CThostFtdcOrderActionField));
	data += sizeof(CThostFtdcOrderActionField);

	CThostFtdcRspInfoField *pRspInfoField = (CThostFtdcRspInfoField*)(data);
	memcpy(pRspInfoField, pRspInfo, sizeof(CThostFtdcRspInfoField));

	responsePublisher.send(rep);
	
    BOOST_LOG_SEV(lg, info) << "OnErrRtnOrderAction --" << FuturesUtil::futuresOrderActionFieldToString(p) << FuturesUtil::futuresRspInfoToString(pRspInfo);
}

void FuturesTradingServer::OnRtnOrder(CThostFtdcOrderField * p) {

	zmq::message_t rep(sizeof(MSG_TYPE)+sizeof(CThostFtdcOrderField));
	*((MSG_TYPE*)rep.data()) = REP_RTN_ORDER;

	CThostFtdcOrderField *pOrderField = (CThostFtdcOrderField*)((char*)rep.data() + sizeof(MSG_TYPE));
	memcpy(pOrderField, p, sizeof(CThostFtdcOrderField));

	responsePublisher.send(rep);

	BOOST_LOG_SEV(lg, info) << "OnRtnOrder --" << FuturesUtil::futuresOrderFieldToString(p);
}

void FuturesTradingServer::OnErrRtnOrderInsert(CThostFtdcInputOrderField *p, CThostFtdcRspInfoField *pRspInfo) {

	zmq::message_t rep(sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderField)+sizeof(CThostFtdcRspInfoField));
	*((MSG_TYPE*)rep.data()) = REP_ERR_RTN_ORDER_INSERT;

	CThostFtdcInputOrderField *pOrderInputField = (CThostFtdcInputOrderField*)((char*)rep.data() + sizeof(MSG_TYPE));
	memcpy(pOrderInputField, p, sizeof(CThostFtdcInputOrderField));

	CThostFtdcRspInfoField *pRspInfoField = (CThostFtdcRspInfoField*)((char*)rep.data() + sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderField));
	memcpy(pRspInfoField, pRspInfo, sizeof(CThostFtdcRspInfoField));

	responsePublisher.send(rep);

    BOOST_LOG_SEV(lg, info) << "OnErrRtnOrderInsert--" << FuturesUtil::futuresInputOrderFieldToString(p) << FuturesUtil::futuresRspInfoToString(pRspInfo);
}

void FuturesTradingServer::OnRspOrderInsert(CThostFtdcInputOrderField *p, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

	zmq::message_t rep(sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderField)+sizeof(CThostFtdcRspInfoField)+sizeof(int)+sizeof(int));
	*((MSG_TYPE*)rep.data()) = REP_RSP_ORDER_INSERT;

	CThostFtdcInputOrderField *pOrderInputField = (CThostFtdcInputOrderField*)((char*)rep.data() + sizeof(MSG_TYPE));
	memcpy(pOrderInputField, p, sizeof(CThostFtdcInputOrderField));

	CThostFtdcRspInfoField *pRspInfoField = (CThostFtdcRspInfoField*)((char*)rep.data() + sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderField));
	memcpy(pRspInfoField, pRspInfo, sizeof(CThostFtdcRspInfoField));

	int *pNRequestID = (int*)((char*)rep.data() + sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderField)+sizeof(CThostFtdcRspInfoField));
	*pNRequestID = nRequestID;

	int *pIsLast = (int*)((char*)rep.data() + sizeof(MSG_TYPE)+sizeof(CThostFtdcInputOrderField)+sizeof(CThostFtdcRspInfoField)+sizeof(int));
	*pIsLast = bIsLast ? 1 : 0;

	responsePublisher.send(rep);

	BOOST_LOG_SEV(lg, info) << "OnRspOrderInsert--" << FuturesUtil::futuresInputOrderFieldToString(p) << FuturesUtil::futuresRspInfoToString(pRspInfo);
}
