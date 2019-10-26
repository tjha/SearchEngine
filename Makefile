CXX = g++

# path #
SRC_PATH = src
BUILD_PATH = build
INCLUDE = -I./utils/

# flags #
CXXFLAGS = -std=c++17 -Wall -Wextra -g

build_sources: src/*
	@mkdir -p $(BUILD_PATH)/

build_tests: build_vector_tests build_parser_tests 

build_vector_tests:
	$(CXX) $(CXXFLAGS) $(INCLUDE) tst/main.cpp tst/vectorTests.cpp -o build/vectorTests.exe

build_parser_tests:
	$(CXX) $(CXXFLAGS) $(INCLUDE) tst/main.cpp tst/basicParserTests.cpp -o build/basicParserTests.exe

run_unit_tests: run_vector_tests run_parser_tests

run_vector_tests:
	./$(BUILD_PATH)/vectorTests.exe

run_parser_tests:
	./$(BUILD_PATH)/basicParserTests.exe

# TODO: run_integration_tests #

clean:
	@rm -rf $(BUILD_PATH)/

.PHONY: build_sources build_tests, run_unit_tests, clean

