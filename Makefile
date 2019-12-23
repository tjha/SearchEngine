CXX := g++

# Where our .cpp and .hpp files are
SOURCE_DIR := src
# Where the Catch 2 framework files are found
TEST_DIR := tst
# Where to output
BUILD_DIR := build

# TODO: -Werror
ifeq ($(tls),no)
	CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -g3 -pthread
else
	CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -g3 -pthread -ltls
endif

# TODO: Is this block necessary?
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CXXFLAGS := $(CXXFLAGS) -L/opt/libressl/lib
	CXXFLAGS := $(CXXFLAGS) -I/opt/libressl/include
endif
ifeq ($(UNAME_S),Darwin)
	CXXFLAGS := $(CXXFLAGS) -L/usr/local/opt/libressl/lib
	CXXFLAGS := $(CXXFLAGS) -I/usr/local/opt/libressl/include
endif

CXXFLAGS := $(CXXFLAGS) -I $(SOURCE_DIR)/ -I $(TEST_DIR)/

# .hpp files
HEADER_SOURCES := $(shell find $(SOURCE_DIR) -type f -name '*.hpp')
# .cpp files
BUILD_SOURCES := $(shell find $(SOURCE_DIR) -type f -name '*.cpp')
# .exe files corresponding to TEST_SOURCES. This is pretty contrived, but make wasn't playing nice otherwise
ALL_TEST_EXECUTABLES :=\
		$(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.exe,$(shell find $(SOURCE_DIR) -type f -name '*.test.cpp'))
TEST_EXECUTABLES := $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.exe,$(case))
$(info $(case))
$(info $(TEST_EXECUTABLES))
ifndef $(case)
	case := all
endif
$(info $(TEST_EXECUTABLES))
ifeq ($(case),all)
	TEST_EXECUTABLES := $(ALL_TEST_EXECUTABLES)
endif
$(info $(TEST_EXECUTABLES))
# .o files that will be made (and should be kept)
PRECIOUS_OBJECTS := $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(BUILD_SOURCES))

all: test

test: $(TEST_EXECUTABLES)
# Don't run the indexer tests for now
	@for file in $^; do\
		if test "$$file" = "build/indexer/index.test.exe"; then\
			echo "skipping test";\
		else\
			./$$file;\
		fi;\
	done;

# Can use this for some testing
noop:
	$(info Doing nothing!)

$(BUILD_DIR)/%.test.exe: $(BUILD_DIR)/%.test.o $(BUILD_DIR)/main.o
# Also compile the corresponding .cpp (not .test.cpp) file if it exists
	@if test -f $(patsubst $(BUILD_DIR)/%.test.exe,$(SOURCE_DIR)/%.cpp,$@); then\
		make $(patsubst %.test.exe,%.o,$@);\
		$(CXX) $(CXXFLAGS) $^ $(patsubst %.test.exe,%.o,$@) -o $@;\
	else\
		$(CXX) $(CXXFLAGS) $^ -o $@;\
	fi;

# Need to special case queryCompiler
$(BUILD_DIR)/queryCompiler/queryCompiler.test.exe: $(BUILD_DIR)/queryCompiler/queryCompiler.test.o $(BUILD_DIR)/main.o\
		$(BUILD_DIR)/queryCompiler/expression.o $(BUILD_DIR)/queryCompiler/parser.o\
		$(BUILD_DIR)/queryCompiler/tokenstream.o $(BUILD_DIR)/constraintSolver/constraintSolver.o\
		$(BUILD_DIR)/indexer/index.o
	$(CXX) $(CXXFLAGS) $^ -o $@;

$(BUILD_DIR)/ranker/ranker.test.exe: $(BUILD_DIR)/ranker/ranker.test.o $(BUILD_DIR)/main.o\
		$(BUILD_DIR)/queryCompiler/expression.o $(BUILD_DIR)/queryCompiler/parser.o\
		$(BUILD_DIR)/queryCompiler/tokenstream.o $(BUILD_DIR)/constraintSolver/constraintSolver.o\
		$(BUILD_DIR)/indexer/index.o
	$(CXX) $(CXXFLAGS) $^ -o $@;

# Need to special case main.o for Catch-2
$(BUILD_DIR)/main.o: $(TEST_DIR)/main.cpp $(TEST_DIR)/catch.hpp
	@mkdir -vp $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@mkdir -vp $(@D)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

# When any .hpp file is changed, assume we should rebuild everything (maybe bad, but it keeps this file shorter)
$(SOURCE_DIR)/%.cpp: $(HEADER_SOURCES)

# MODULE_CASES := $(wildcard $(TEST_PATH)/$(module)/*.cpp)
# MODULE_TESTS := $(patsubst $(TEST_PATH)/%.tests.cpp,$(BUILD_PATH)/tst/%.tests.exe,$(MODULE_CASES))

# all: print_os $(TESTS)

# crawlerDriver: src/driver/driver.cpp
# 	make build
# 	$(CXX) $(CXXFLAGS) src/driver/driver.cpp $(INCLUDES) -ltls -O3 -o $(BUILD_PATH)/driver.exe

# chunkTest: src/frontend/testChunks.cpp
# 	make build
# 	$(CXX) $(CXXFLAGS) src/frontend/testChunks.cpp src/indexer/index.cpp src/constraintSolver/constraintSolver.cpp src/queryCompiler/parserQC.cpp src/queryCompiler/expression.cpp src/queryCompiler/tokenstream.cpp $(INCLUDES) -ltls -O3 -o $(BUILD_PATH)/chunkDriver.exe

# indexerDriver: src/indexer/driver.cpp src/indexer/index.cpp
# 	make build
# 	$(CXX) $(CXXFLAGS) src/indexer/driver.cpp src/indexer/index.cpp $(INCLUDES) -ltls -O3 -o $(BUILD_PATH)/indexerDriver.exe

# indexerTest: tst/indexer/indexTests.cpp
# 	make build
# 	$(CXX) $(CXXFLAGS) tst/indexer/indexTests.cpp $(INCLUDES) tst/main.cpp -O3 -o $(BUILD_PATH)/indexTests.exe

# driverFinal: src/driver/driver.cpp
# 	make build
# 	$(CXX) $(CXXFLAGS) src/driver/driver.cpp $(INCLUDES) -ltls -O3 $(BUILD_PATH)/driver.exe

# multithreadingTest: src/driver/multithreadingTest.cpp
# 	$(CXX) $(CXXFLAGS) src/driver/multithreadingTest.cpp $(INCLUDES) -ltls -o build/multithreadingTest.exe

# cleanDriver:
# 	@rm -rf data/tmp/logs/
# 	@rm -rf data/tmp/performance/

# test: $(BUILD_PATH)/$(case).tests.exe

# tests: $(MODULE_TESTS)

# build:
# 	@mkdir -vp $(addprefix $(BUILD_PATH)/tst/,$(TESTS_PATHS))

# $(BUILD_PATH)/tst/%.tests.exe: $(BUILD_PATH)/tst/%.tests.o
# 	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ tst/main.cpp -o $@ -ltls
# 	./$@
# 	make clean

# $(BUILD_PATH)/tst/%.tests.o: $(TEST_PATH)/%.tests.cpp
# 	make build
# 	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@ -ltls

# print_os: ;@echo 'os = ' $(OSFLAG)

# TODO: run_integration_tests #

clean:
	@rm -rf $(BUILD_DIR)/

.PHONY: all test clean noop

# Keep all of our object files aroud for future compilations
.PRECIOUS: $(PRECIOUS_OBJECTS)
