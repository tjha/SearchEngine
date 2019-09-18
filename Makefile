CXX = g++

# path #
SRC_PATH = src
BUILD_PATH = build
INCLUDE = -I./utils/

# flags #
CXXFLAGS = -std=c++17 -Wall -Wextra -g

build_sources: src/*
	@mkdir -p $(BUILD_PATH)/

build_tests: tst/*
	@mkdir -p $(BUILD_PATH)/
	$(CXX) $(CXXFLAGS) $(INCLUDE) tst/vector_tests.cpp -o build/vector_tests

run_unit_tests:
	./$(BUILD_PATH)/vector_tests

# TODO: run_integration_tests #

clean:
	@rm -rf $(BUILD_PATH)/

.PHONY: build_sources build_tests, run_unit_tests, clean

