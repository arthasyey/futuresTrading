#include "tradeHandler.h"
#pragma comment(lib, "zmq.lib")


int main(int argc, char* argv[]) {
	FuturesConfigInfo configInfo = (argc == 2) ? FuturesUtil::LoadConfigureFile(string(argv[1])) : FuturesUtil::LoadConfigureFile("citicSimu.txt");

	FuturesTradingServer server(configInfo);
	server.initTradingServer();
	server.run();

}
