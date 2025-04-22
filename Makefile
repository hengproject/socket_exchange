CXX = g++
CXXFLAGS = -Wall -std=c++11


all: serverM

serverM: serverM.cpp common_variables.h serverM.h
	$(CXX) $(CXXFLAGS) -o serverM serverM.cpp

serverA: serverA.cpp common_variables.h serverA.h
	$(CXX) $(CXXFLAGS) -o serverA serverA.cpp

serverP: serverP.cpp common_variables.h serverP.h
	$(CXX) $(CXXFLAGS) -o serverP serverP.cpp

serverQ: serverQ.cpp common_variables.h serverQ.h
	$(CXX) $(CXXFLAGS) -o serverQ serverQ.cpp

client: client.cpp common_variables.h client.h
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
	rm -f serverM

