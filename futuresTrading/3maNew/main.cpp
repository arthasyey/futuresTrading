#include "3MANew.h"

int main(int argc, char* argv[]) {
  initLogger("130//130_%Y%m%d_%N.log");
  FuturesConfigInfo configInfo = (argc == 2) ? FuturesUtil::LoadConfigureFile(string(argv[1])) : FuturesUtil::LoadConfigureFile("citicSimu.txt");

  ThreeMAClass<PROD> instance(configInfo);
  instance.run();

  return 0;
}
