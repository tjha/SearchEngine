CXX = g++
CXXFLAGS = -std=c++11 -g

LDFLAGS = -L/usr/local/opt/libressl/lib
CPPFLAGS = -I/usr/local/opt/libressl/include

crawler: crawler.cpp
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) $(CPPFLAGS) -ltls -v -o crawler.exe

ssl_test: ssl_test.cpp
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) $(CPPFLAGS) -o ssl_test.exe

testRobots: testRobots.cpp
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) $(CPPFLAGS) -o testRobots.exe

.PHONY: clean

clean:
	rm -f ssl_test.exe
	rm -f crawler.exe
	rm -f testRobots.exe
