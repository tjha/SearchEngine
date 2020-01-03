CXX := g++

# Where our .cpp and .hpp files are
SOURCE_DIR := src
# Where the Catch 2 framework files are found
TEST_DIR := tst
# Where to output
BUILD_DIR := build

CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -g3 -pthread -ltls
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

# TODO: -Werror
ifeq ($(tls),no)
	CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -g3 -pthread
endif

CXXFLAGS := $(CXXFLAGS) -I $(SOURCE_DIR)/ -I $(TEST_DIR)/

# .hpp files
HEADER_SOURCES := $(shell find $(SOURCE_DIR) -type f -name '*.hpp')
# .cpp files
BUILD_SOURCES := $(shell find $(SOURCE_DIR) -type f -name '*.cpp')
# .exe files corresponding to TEST_SOURCES. This is pretty contrived, but make wasn't playing nice otherwise
ALL_TEST_EXECUTABLES :=\
		$(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.exe,$(shell find $(SOURCE_DIR) -type f -name '*.test.cpp'))
TESTS_TO_SKIP := $(BUILD_DIR)/indexer/index.test.exe $(BUILD_DIR)/crawler/crawler.test.exe
TEST_EXECUTABLES := $(patsubst $(SOURCE_DIR)/%,$(BUILD_DIR)/%.test.exe,$(case))
ifndef $(case)
	case := all
endif
ifeq ($(case),all)
	TEST_EXECUTABLES := $(filter-out $(TESTS_TO_SKIP),$(ALL_TEST_EXECUTABLES))
endif
# .o files that will be made (and should be kept)
PRECIOUS_OBJECTS := $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(BUILD_SOURCES))

all: test

test: $(TEST_EXECUTABLES)
# Don't run the indexer or crawler tests for now
	@for file in $^; do\
		echo ./$$file;\
		./$$file;\
	done;

# Can use this for some testing of the Makefile
noop:
	$(info Doing nothing!)

# Need to special case queryCompiler and ranker
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
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(HEADER_SOURCES)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $(filter-out %.hpp,$^) -o $@

crawlerDriver: $(SOURCE_DIR)/driver/driver.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -O3 $^ -o $(BUILD_DIR)/driver.exe

indexerDriver: $(SOURCE_DIR)/indexer/driver.o $(SOURCE_DIR)/indexer/index.o
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -O3 $^ -o $(BUILD_DIR)/indexerDriver.exe

PORT=8000
server: $(SOURCE_DIR)/frontend/httpServer.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -O3 $^ -o $(BUILD_DIR)/server.exe
	./$(BUILD_DIR)/server.exe $(EXECUTABLE)

printOS:
	$(info Your OS is '$(UNAME_S)')

clean:
	@rm -rf $(BUILD_DIR)/

cleanDriver:
	@rm -rf data/tmp/logs/
	@rm -rf data/tmp/performance/

help:
	@echo "To make all of the tests, just run make test or make test case=all"
	@echo "To run a specific test, run something similar to make test case=src/constraintSolver/constraintSolver. Note that the thing after the equals sign should be the name of a .test.cpp file without its extension"
	@echo "To make the drivers, run make crawlerDriver and make indexerDriver"
	@echo "To clean things up, run make clean (destroys the build/ directory) and/or make cleanDriver (destroys the data/ directory)"
	@echo "To disable the use of the LibreSSL library (which is good if you don't have it installed), add the flag tls=no into your make command (i.e. something like make test tls=no)."

.PHONY: all test noop crawlerDriver indexerDriver printOS clean cleanDriver help

# Keep all of our object files aroud for future compilations
.PRECIOUS: $(PRECIOUS_OBJECTS)

.SECONDEXPANSION:
$(BUILD_DIR)/%.test.exe: $(BUILD_DIR)/%.test.o $(BUILD_DIR)/main.o $$(patsubst $(SOURCE_DIR)/$%.cpp,$(BUILD_DIR)/$%.o,$$(wildcard $(SOURCE_DIR)/$$*.cpp))
# Also compile the corresponding .cpp (not .test.cpp) file if it exists
	$(CXX) $(CXXFLAGS) $^ -o $@;
