CC=g++
CFLAGS:=-std=gnu++11 -g -o3 -c -Wall 
LDFLAGS=
SOURCES=main.cpp tradeHandler.cpp ../src/FuturesUtil.cpp ../src/logging.cpp ../src/FuturesTraderSpi.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=tradeHandler

LIBS += -L/usr/local/lib/ -L../libs -L/usr/local/Cellar/boost/1.58.0/lib/ -lzmq -lpthread -lboost_thread -lboost_system -lboost_filesystem -lboost_date_time -lboost_thread -lboost_log_setup -lboost_log
INCLUDES += -I/usr/local/Cellar/boost/1.58.0/include/

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) -DBOOST_ALL_DYN_LINK $(LDFLAGS) $(LIBS) $(OBJECTS)  ../ctp/thosttraderapi.so -o $@

.cpp.o:
	$(CC) -DBOOST_ALL_DYN_LINK $(CFLAGS) $(INCLUDES) $< -o $@


clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)
