CXX = g++
CXXFLAGS = -std=c++11

LDFLAGS = -L/usr/local/opt/libressl/lib
CPPFLAGS = -I/usr/local/opt/libressl/include

crawlerTests: crawlerTests.cpp
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) $(CPPFLAGS) -ltls -o crawlerTests.exe

.PHONY: clean

clean:
	rm -f ssl_test.exe
	rm -f crawler.exe
	rm -f testRobots.exe
