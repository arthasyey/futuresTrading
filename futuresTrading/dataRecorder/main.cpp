#include "dataRecorder.h"

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>


int main(int argc, char* argv[]) {
  initLogger("Recorder//Recorder_%Y%m%d_%N.log");
  FuturesConfigInfo configInfo = (argc == 2) ? FuturesUtil::LoadConfigureFile(string(argv[1])) : FuturesUtil::LoadConfigureFile("/marketData/shenwan.txt");

  DataRecorder dataRecorder(configInfo);
  //dataRecorder.initTables();

  int i;
  cin >> i;
};
