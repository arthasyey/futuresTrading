CC=g++
CFLAGS:=-std=gnu++11 -g -o3 -c -Wall -Wno-unused-function -fPIC
LDFLAGS=
SOURCES=main.cpp tradeHandler.cpp ../src/FuturesUtil.cpp ../src/logging.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=tradeHandler.exe

LIBS += -L../libs -lboost_system -lboost_filesystem -lboost_date_time -lboost_thread -lboost_log_setup -lboost_log -lpthread -ldl -lthosttraderapi -lzmq
INCLUDES += -I../include -Wl,-rpath=.

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) -DBOOST_ALL_DYN_LINK $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

.cpp.o:
	$(CC) -DBOOST_ALL_DYN_LINK $(CFLAGS) $(INCLUDES) $< -o $@


clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)
