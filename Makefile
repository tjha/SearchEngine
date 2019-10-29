CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# path #
SRC_PATH = src
TEST_PATH = tst
BUILD_PATH = build
INCLUDE = -I $(SRC_PATH)/utils/

TEST_SOURCES := $(wildcard $(TEST_PATH)/*.cpp)    
TESTS := $(patsubst $(TEST_PATH)/%Tests.cpp,$(BUILD_PATH)/tst/%Tests.exe,$(TEST_SOURCES))

tests: $(TESTS)

$(BUILD_PATH)/tst/%Tests.exe: $(TEST_PATH)/%Tests.o
	@mkdir -p $(BUILD_PATH)/tst
	$(CXX) $^ tst/main.cpp -o $@
	./$@

$(BUILD_PATH)/tst/%Tests.o: $(TEST_PATH)/%Tests.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

# TODO: run_integration_tests #

clean:
	@rm -rf $(BUILD_PATH)/

.PHONY: tests clean

