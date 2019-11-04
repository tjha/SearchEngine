CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

LDFLAGS := -L/usr/local/opt/libressl/lib
CPPFLAGS := -I/usr/local/opt/libressl/include
INCLUDE := -I$(SRC_PATH)/utils/

crawlerTests: crawlerTests.cpp robots.hpp 
	$(CXX) $(CXXFLAGS) crawlerTests.cpp $(LDFLAGS) $(CPPFLAGS) $(INCLUDE) -ltls -o crawlerTests.exe

crawl: 	
	$(CXX) $(CXXFLAGS) crawl.cpp $(LDFLAGS) $(CPPFLAGS) $(INCLUDE) -ltls -o crawl.exe
	./crawl.exe seedlist.txt savedRobots.txt savedCrawl.txt

.PHONY: clean

clean:
	rm -f crawlerTests.exe
	rm -f crawler.exe
