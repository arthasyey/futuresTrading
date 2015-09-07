#include "feedHandler.h"


FeedHandler::FeedHandler(const FuturesConfigInfo& _configInfo) : config(_configInfo), context(), feedPublisher(context, ZMQ_PUB) {
  pMdApi = CThostFtdcMdApi::CreateFtdcMdApi();
  pMdApi->RegisterSpi(this);
  pMdApi->Init();
  pMdApi->RegisterFront(const_cast<char*>(config.MdFrontAddr.c_str()));
  feedPublisher.bind("tcp:///tmp/futuresFeeds");
}

void FeedHandler::OnFrontConnected() {
  CThostFtdcReqUserLoginField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, config.MdBrokerId.c_str());
  strcpy(req.UserID, config.MdUserId.c_str());
  strcpy(req.Password, config.MdPassword.c_str());
  int iResult = pMdApi->ReqUserLogin(&req, 0);
  LOG_BOOST << ((iResult == 0) ? "success" : "failure") << endl;
}

void FeedHandler::OnFrontDisconnected(int nReason) {
  LOG_BOOST << nReason;
  pMdApi->RegisterFront(const_cast<char*>(config.MdFrontAddr.c_str()));
}

void FeedHandler::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  cerr << "--->>> " << "FuturesMdSpi::OnRspUserLogin" << endl;
  if (bIsLast)
  {
    LOG_BOOST << "subscribe to symbols: " << config.Symbol << endl;
    SubscribeMarketData(config.Symbol);
  }
}

int FeedHandler::SubscribeMarketData(const vector<string>& instruments) {
  int count = instruments.size();
  char** pInstruments = new char*[count];
  for (int i = 0; i < count; ++i)
  {
    pInstruments[i] = new char[20];
    strcpy(pInstruments[i], instruments[i].c_str());
  }

  int ret = pMdApi->SubscribeMarketData(pInstruments, instruments.size());
  for (int i = 0; i < count; ++i)
    delete pInstruments[i];
  delete pInstruments;

  return ret;
}

int FeedHandler::SubscribeMarketData(const string& instrument) {
  vector<string> instruments(1, instrument);
  return SubscribeMarketData(instruments);
}

void FeedHandler::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {
  LOG_BOOST;
  zmq::message_t rep(sizeof(MSG_TYPE)+sizeof(CThostFtdcDepthMarketDataField));
  char* data = (char*)rep.data();

  *((MSG_TYPE*)data) = MSG_FEED;
  data += sizeof(MSG_TYPE);

  CThostFtdcDepthMarketDataField *p = (CThostFtdcDepthMarketDataField*)(data);
  memcpy(p, pDepthMarketData, sizeof(CThostFtdcDepthMarketDataField));

  feedPublisher.send(rep);
}

