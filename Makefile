CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g3 -pthread

# flags
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	OSFLAG := -D LINUX
	LDFLAGS := -L/opt/libressl/lib
	CPPFLAGS := -I/opt/libressl/include
endif
ifeq ($(UNAME_S),Darwin)
	OSFLAG := -D OSX
	LDFLAGS := -L/usr/local/opt/libressl/lib
	CPPFLAGS := -I/usr/local/opt/libressl/include
endif

# path #
SRC_PATH = src
TEST_PATH = tst
BUILD_PATH = build
INCLUDES = -I $(SRC_PATH)/utils/ -I $(SRC_PATH)/parser/ -I $(SRC_PATH)/crawler/ -I $(SRC_PATH)/indexer/ -I $(SRC_PATH)/driver/ -I $(SRC_PATH)/ranker/ -I $(SRC_PATH)/constraintSolver/ -I $(SRC_PATH)/queryCompiler/ -I $(TEST_PATH) $(LDFLAGS) $(CPPFLAGS)

TEST_SOURCES := $(wildcard $(TEST_PATH)/*/*.cpp)
TESTS := $(patsubst $(TEST_PATH)/%Tests.cpp,$(BUILD_PATH)/tst/%Tests.exe,$(TEST_SOURCES))
TESTS_PATHS := $(filter-out $(TEST_PATH)/%.cpp $(TEST_PATH)/%.hpp, $(wildcard $(TEST_PATH)/*/))
TESTS_PATHS := $(patsubst $(TEST_PATH)/%,%,$(TESTS_PATHS))

MODULE_CASES := $(wildcard $(TEST_PATH)/$(module)/*.cpp)
MODULE_TESTS := $(patsubst $(TEST_PATH)/%Tests.cpp,$(BUILD_PATH)/tst/%Tests.exe,$(MODULE_CASES))

all: print_os $(TESTS)

crawlerDriver: src/driver/driver.cpp
	make build
	$(CXX) $(CXXFLAGS) src/driver/driver.cpp $(INCLUDES) -ltls -O3 -o $(BUILD_PATH)/driver.exe

chunkTest: src/frontend/testChunks.cpp
	make build
	$(CXX) $(CXXFLAGS) src/frontend/testChunks.cpp src/indexer/index.cpp src/constraintSolver/constraintSolver.cpp src/queryCompiler/parserQC.cpp src/queryCompiler/expression.cpp src/queryCompiler/tokenstream.cpp $(INCLUDES) -ltls -O3 -o $(BUILD_PATH)/chunkDriver.exe

indexerDriver: src/indexer/driver.cpp src/indexer/index.cpp
	make build
	$(CXX) $(CXXFLAGS) src/indexer/driver.cpp src/indexer/index.cpp $(INCLUDES) -ltls -O3 -o $(BUILD_PATH)/indexerDriver.exe

indexerEncodingTest: tst/indexer/indexerEncodingTests.cpp
	make build
	$(CXX) $(CXXFLAGS) tst/indexer/indexerEncodingTests.cpp $(INCLUDES) -ltls -O3 -o $(BUILD_PATH)/encodingTests.exe

indexerTest: tst/indexer/indexTests.cpp
	make build
	$(CXX) $(CXXFLAGS) tst/indexer/indexTests.cpp $(INCLUDES) tst/main.cpp -O3 -o $(BUILD_PATH)/indexTests.exe

driverFinal: src/driver/driver.cpp
	make build
	$(CXX) $(CXXFLAGS) src/driver/driver.cpp $(INCLUDES) -ltls -O3 $(BUILD_PATH)/driver.exe

multithreadingTest: src/driver/multithreadingTest.cpp
	$(CXX) $(CXXFLAGS) src/driver/multithreadingTest.cpp $(INCLUDES) -ltls -o build/multithreadingTest.exe

cleanDriver:
	@rm -rf data/tmp/logs/
	@rm -rf data/tmp/performance/

test: $(BUILD_PATH)/tst/$(case)Tests.exe

tests: $(MODULE_TESTS)

build:
	@mkdir -vp $(addprefix $(BUILD_PATH)/tst/,$(TESTS_PATHS))

$(BUILD_PATH)/tst/%Tests.exe: $(BUILD_PATH)/tst/%Tests.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ tst/main.cpp -o $@ -ltls
	./$@
	make clean

$(BUILD_PATH)/tst/%Tests.o: $(TEST_PATH)/%Tests.cpp
	make build
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@ -ltls

print_os: ;@echo 'os = ' $(OSFLAG)

# TODO: run_integration_tests #

clean:
	@rm -rf $(BUILD_PATH)/

.PHONY: tests clean cleanDriver

