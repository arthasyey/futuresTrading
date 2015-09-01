#include "../dataRecorder/dataRecorder.h"

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>

using namespace boost::filesystem;

int main(int argc, char* argv[]) {
  string strs[] = {"IF1501", "IF1502", "IF1503", "IF1504", "IF1505", "IF1506", "IF1507", "IF1509", "IF1512"};

  for(string s : strs) {
    path p("../datas/" + s);
    for(auto& entry : boost::make_iterator_range(directory_iterator(p), {})) {
      string filePath = entry.path().string();
         std::cout << filePath << "\n";
         size_t pos_end = filePath.find("min");
         if(pos_end != string::npos) {
           size_t pos_start = filePath.find("_");
           int period = atoi(filePath.substr(pos_start+1, pos_end - pos_start-1).c_str());
           string date = filePath.substr(pos_start-8, 8);

           string query = (boost::format(CREATE_KLINE_TABLE_QUERY) % date).str();
           mysqlConnector.executeUpdate(query);

           ifstream ifs(filePath.c_str());
           string line;
           getline(ifs, line);
           while(getline(ifs, line)) {
            vector<string> segments = FuturesUtil::split(line, ',');
            string& time = segments[0];
            double open = atof(segments[1].c_str());
            double high = atof(segments[2].c_str());
            double low = atof(segments[3].c_str());
            double close = atof(segments[4].c_str());
            int volume = atoi(segments[5].c_str());
            string qry = (boost::format(INSERT_KLINE_QUERY) % date % s % period % time % open % high % low % close % volume).str();
            mysqlConnector.executeUpdate(qry);
           }
         }
    }
  }
};
