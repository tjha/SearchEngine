CXX = g++

# path #
SRC_PATH = src
TEST_PATH = tst
BUILD_PATH = build
INCLUDE = -I $(SRC_PATH)/utils/

# flags #
CXXFLAGS = -std=c++17 -Wall -Wextra -g

build_sources: $(SRC_PATH)/*
	@mkdir -p $(BUILD_PATH)/

build_tests: $(TEST_PATH)/*
	@mkdir -p $(BUILD_PATH)/
	$(CXX) $(CXXFLAGS) $(INCLUDE) tst/main.cpp tst/vectorTests.cpp -o build/vectorTests.exe

run_unit_tests:
	./$(BUILD_PATH)/vectorTests.exe

# TODO: run_integration_tests #

clean:
	@rm -rf $(BUILD_PATH)/

.PHONY: build_sources build_tests run_unit_tests clean

