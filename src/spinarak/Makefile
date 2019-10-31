CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

LDFLAGS := -L/usr/local/opt/libressl/lib
CPPFLAGS := -I/usr/local/opt/libressl/include
INCLUDE := -I$(SRC_PATH)/utils/

crawlerTests: crawlerTests.cpp robots.hpp 
	$(CXX) $(CXXFLAGS) crawlerTests.cpp $(LDFLAGS) $(CPPFLAGS) $(INCLUDE) -ltls -o crawlerTests.exe

.PHONY: clean

clean:
	rm -f crawlerTests.exe
