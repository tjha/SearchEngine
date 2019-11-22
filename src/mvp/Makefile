CXX = g++
CXXFLAGS = -std=c++11

LDFLAGS := -L/usr/local/opt/libressl/lib
CPPFLAGS := -I/usr/local/opt/libressl/include
INCLUDE := -I$(SRC_PATH)/utils/

driver: driver.cpp
	$(CXX) $(CXXFLAGS) driver.cpp $(LDFLAGS) $(CPPFLAGS) $(INCLUDE) -ltls -pthread -g -o driver.exe

.PHONY: clean

clean:
	rm -f driver.exe
	rm logs/*
	rm -r html/*
