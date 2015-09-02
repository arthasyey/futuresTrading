#include "feedHandler.h"


int main(int argc, char * argv[]) {
  initLogger("FeedHandler//FeedHandler_%Y%m%d_%N.log");
  FuturesConfigInfo configInfo = (argc == 2) ? FuturesUtil::LoadConfigureFile(string(argv[1])) : FuturesUtil::LoadConfigureFile("/marketData/shenwan.txt");
  FeedHandler server(configInfo);

  int i;
  cin >> i;
}
