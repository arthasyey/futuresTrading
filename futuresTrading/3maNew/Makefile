CC=g++
CFLAGS:=-std=gnu++11 -g -c -Wall -Wno-unused-function -fPIC
LDFLAGS=

SOURCES=main.cpp ../src/FuturesUtil.cpp ../src/logging.cpp ../src/kLineGenerator.cpp ../src/mysqlConnector.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=3ma.exe

SIM_SOURCES=main_sim.cpp ../src/FuturesUtil.cpp ../src/logging.cpp ../src/kLineGenerator.cpp ../src/mysqlConnector.cpp
SIM_OBJECTS=$(SIM_SOURCES:.cpp=.o)
SIM_EXECUTABLE=3ma_sim.exe

LIBS += -L../libs -lboost_system -lboost_filesystem -lboost_date_time -lboost_thread -lboost_log_setup -lboost_log -lpthread -ldl -lzmq -lmysqlcppconn -ljson-c
INCLUDES += -I../include -I../ctp -Wl,-rpath=.

all: $(SOURCES) $(SIM_EXECUTABLE) $(EXECUTABLE)  
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) -DBOOST_ALL_DYN_LINK $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

$(SIM_EXECUTABLE): $(SIM_OBJECTS) 
	$(CC) -DBOOST_ALL_DYN_LINK $(LDFLAGS) $(SIM_OBJECTS) $(LIBS) -o $@

.cpp.o:
	$(CC) -DBOOST_ALL_DYN_LINK $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	$(RM) $(OBJECTS) $(EXECUTABLE) $(SIM_EXECUTABLE)
