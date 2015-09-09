#include "dataRecorder.h"

#include <thread>
#include <chrono>
#include <ctime>


void terminateThread() {
  using std::chrono::system_clock;
  std::time_t tt = system_clock::to_time_t (system_clock::now());

  struct std::tm * ptm = std::localtime(&tt);

  ptm->tm_hour = 15;
  ptm->tm_min = 30;
  ptm->tm_sec=0;
  std::this_thread::sleep_until (system_clock::from_time_t (mktime(ptm)));
}


int main(int argc, char* argv[]) {
  initLogger("Recorder//Recorder_%Y%m%d_%N.log");
  FuturesConfigInfo configInfo = (argc == 2) ? FuturesUtil::LoadConfigureFile(string(argv[1])) : FuturesUtil::LoadConfigureFile("/marketData/shenwan.txt");

  thread t(terminateThread);
  DataRecorder dataRecorder(configInfo);

  t.join();

  cout << "15:30 has reached!\n";
  dataRecorder.finalize();
};
