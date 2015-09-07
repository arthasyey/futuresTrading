CC=g++
CFLAGS:=-std=gnu++11 -g -o3 -c -Wall -fPIC
LDFLAGS=
SOURCES=main.cpp dataRecorder.cpp ../src/FuturesUtil.cpp ../src/logging.cpp ../src/mysqlConnector.cpp ../src/kLineGenerator.cpp ../src/json.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=dataRecorder.exe

LIBS += -L../libs -lboost_system -lboost_filesystem -lboost_date_time -lboost_thread -lboost_log_setup -lboost_log -lpthread -ldl -ljson-c -lmysqlcppconn -lthostmduserapi -lthosttraderapi  
INCLUDES += -I../ctp -I../include -Wl,-rpath=.

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) -DBOOST_ALL_DYN_LINK $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

.cpp.o:
	$(CC) -DBOOST_ALL_DYN_LINK $(CFLAGS) $(INCLUDES) $< -o $@


clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)
