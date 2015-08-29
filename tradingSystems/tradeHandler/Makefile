CC=g++
CFLAGS:=-std=gnu++11 -g -o3 -c -Wall -Wnon-pod-varargs
LDFLAGS=
SOURCES=main.cpp tradeHandler.cpp ../src/FuturesUtil.cpp ../src/logging.cpp ../src/FuturesTraderSpi.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=tradeHandler

LIBS += -L/usr/local/lib/ -L../libs -L/usr/local/Cellar/boost/1.58.0/lib/ -lboost_log-mt -lboost_thread-mt -lboost_system-mt -lboost_filesystem-mt -lboost_log_setup-mt -lzmq  
INCLUDES += -I/usr/local/Cellar/boost/1.58.0/include/

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) ../ctp/thosttraderapi.so -o $@

.cpp.o:
	$(CC) -v $(CFLAGS) $(INCLUDES) $< -o $@
