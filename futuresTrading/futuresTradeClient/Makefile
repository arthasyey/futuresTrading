CC=g++
CFLAGS:=-std=gnu++11 -g -o3 -c -Wall -Wno-unused-function -fPIC
LDFLAGS=
SOURCES=futuresTradeClient.cpp ../src/FuturesUtil.cpp ../src/logging.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=libfuturesTradeClient.so.1.0

LIBS += -lboost_system -lboost_filesystem -lboost_date_time -lboost_thread -lboost_log_setup -lboost_log -lpthread -ldl -lzmq
INCLUDES += -I../include -I../ctp -Wl,-rpath=.

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) -DBOOST_ALL_DYN_LINK -shared -Wl,-soname,libfuturesTradeClient.so.1.0 $(LDFLAGS)  $(LIBS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) -DBOOST_ALL_DYN_LINK $(CFLAGS) $(INCLUDES) $< -o $@


clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)
