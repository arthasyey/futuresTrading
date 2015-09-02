#include "tradeHandler.h"


int main(int argc, char* argv[]) {
  initLogger("TradeHandler//TradeHandler_%Y%m%d_%N.log");
  FuturesConfigInfo configInfo = (argc == 2) ? FuturesUtil::LoadConfigureFile(string(argv[1])) : FuturesUtil::LoadConfigureFile("/marketData/citicSimu.txt");

  TradeHandler server(configInfo);
  server.run();
}
