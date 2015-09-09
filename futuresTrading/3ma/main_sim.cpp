#include "3maNew.h"

int main(int argc, char* argv[]){
  assert(argc == 3 && "3ma_sim.exe symbol date");

  stringstream ss;
  ss << "130SIM/130SIM_" << argv[2] << "_" << argv[1] << ".log";
  initLogger(ss.str());

  FuturesConfigInfo configInfo;
  configInfo.Symbol = argv[1];
  configInfo.simDate = argv[2];

  ThreeMAClass<SIM> instance(configInfo);
  instance.run();

  return 0;
}
