#include "3maNew.h"

int main(int argc, char* argv[]){
  assert(argc == 3 && "3ma_sim.exe symbol date");

  initLogger("130SIM//130SIM_%Y%m%d_%N.log");
  FuturesConfigInfo configInfo;
  configInfo.Symbol = argv[1];
  configInfo.simDate = argv[2];

  ThreeMAClass<SIM> instance(configInfo);
  instance.run();

  return 0;
}
