CXX = g++
CXXFLAGS = -std=c++11

LDFLAGS = -L/usr/local/opt/libressl/lib
CPPFLAGS = -I/usr/local/opt/libressl/include

crawler: crawler.cpp
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) $(CPPFLAGS) -ltls -o crawler.exe

.PHONY: clean

clean:
	rm -f ssl_test.exe
	rm -f crawler.exe
	rm -f testRobots.exe
