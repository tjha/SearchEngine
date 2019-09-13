CXX = g++

# path #
SRC_PATH = src
BUILD_PATH = build

# flags #
CXXFLAGS = -std=c++17 -Wall -Wextra -g

build: src/example.cpp
	@mkdir -p $(BUILD_PATH)/
	$(CXX) $(CXXFLAGS) src/example.cpp -o build/example

test: build/example
	./build/example

clean:
	@rm -rf $(BUILD_PATH)/

.PHONY: build test clean

