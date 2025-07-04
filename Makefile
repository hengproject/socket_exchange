CXX = g++
CXXFLAGS = -Wall -std=c++11

TARGETS = serverM serverA serverP serverQ client

all: $(TARGETS)


serverM: serverM.cpp serverM.h common_variables.h common_sockets.h
	$(CXX) $(CXXFLAGS) -o serverM serverM.cpp

serverA: serverA.cpp common_variables.h serverA.h
	$(CXX) $(CXXFLAGS) -o serverA serverA.cpp

serverP: serverP.cpp common_variables.h serverP.h
	$(CXX) $(CXXFLAGS) -o serverP serverP.cpp

serverQ: serverQ.cpp common_variables.h serverQ.h
	$(CXX) $(CXXFLAGS) -o serverQ serverQ.cpp

client: client.cpp client.h common_variables.h common_sockets.h
	$(CXX) $(CXXFLAGS) -o client client.cpp

run_client: client
	./client


run_serverQ: serverQ
	./serverQ


run_serverP: serverP
	./serverP


run_serverA: serverA
	./serverA

run_serverM: serverM
	./serverM

clean:
	rm -f $(TARGETS)

