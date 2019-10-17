CXX = g++
CXXFLAGS = -std=c++11 -g

LDFLAGS = -L/usr/local/opt/libressl/lib
CPPFLAGS = -I/usr/local/opt/libressl/include

ssl_test: ssl_test.cpp
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) $(CPPFLAGS) -o ssl_test

.PHONY: clean

clean:
	rm -f ssl_test
