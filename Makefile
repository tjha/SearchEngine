CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# path #
SRC_PATH = src
TEST_PATH = tst
BUILD_PATH = build
INCLUDES = -I $(SRC_PATH)/utils/ -I $(SRC_PATH)/indexer -I $(TEST_PATH) #TODO: add more src folders here as needed

TEST_SOURCES := $(wildcard $(TEST_PATH)/*/*.cpp)
TESTS := $(patsubst $(TEST_PATH)/%Tests.cpp,$(BUILD_PATH)/tst/%Tests.exe,$(TEST_SOURCES))
TESTS_PATHS := $(filter-out $(TEST_PATH)/%.cpp $(TEST_PATH)/%.hpp, $(wildcard $(TEST_PATH)/*/))
TESTS_PATHS := $(patsubst $(TEST_PATH)/%,%,$(TESTS_PATHS))

MODULE_CASES := $(wildcard $(TEST_PATH)/$(module)/*.cpp)
MODULE_TESTS := $(patsubst $(TEST_PATH)/%Tests.cpp,$(BUILD_PATH)/tst/%Tests.exe,$(MODULE_CASES))

all: $(TESTS)

test: $(BUILD_PATH)/tst/$(case)Tests.exe

tests: $(MODULE_TESTS)

build:
	@mkdir -vp $(addprefix $(BUILD_PATH)/tst/,$(TESTS_PATHS))


$(BUILD_PATH)/tst/%Tests.exe: $(BUILD_PATH)/tst/%Tests.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ tst/main.cpp -o $@
	./$@
# make clean

$(BUILD_PATH)/tst/%Tests.o: $(TEST_PATH)/%Tests.cpp
	make build
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
	
# TODO: run_integration_tests #

clean:
	@rm -rf $(BUILD_PATH)/

# .PHONY: tests clean

