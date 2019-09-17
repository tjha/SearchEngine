CXX = g++

# path #
SRC_PATH = src
BUILD_PATH = build
INCLUDE = -I./utils/

# flags #
CXXFLAGS = -std=c++17 -Wall -Wextra -g

build: src/example.cpp
	@mkdir -p $(BUILD_PATH)/
	$(CXX) $(CXXFLAGS) src/example.cpp -o build/example

test: tst/*
	$(CXX) $(CXXFLAGS) $(INCLUDE) tst/vector_tests.cpp -o build/vector_tests
	./$(BUILD_PATH)/vector_tests

clean:
	@rm -rf $(BUILD_PATH)/

.PHONY: build test clean

