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
TEST_EXECUTABLES := $(patsubst $(SOURCE_DIR)/%,$(BUILD_DIR)/%.test.exe,$(case))
ifndef $(case)
	case := all
endif
ifeq ($(case),all)
	TEST_EXECUTABLES := $(ALL_TEST_EXECUTABLES)
endif
# .o files that will be made (and should be kept)
PRECIOUS_OBJECTS := $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(BUILD_SOURCES))

all: test

test: $(TEST_EXECUTABLES)
# Don't run the indexer or crawler tests for now
	@for file in $^; do\
		echo $$file;\
		case $$file in\
			build/indexer/index.test.exe|build/crawler/crawler.test.exe) echo "skipping test";;\
			*)                                                           ./$$file;;\
		esac;\
	done;

# Can use this for some testing of the Makefile
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
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

# When any .hpp file is changed, assume we should rebuild everything (maybe bad, but it keeps this file shorter)
$(SOURCE_DIR)/%.cpp: $(HEADER_SOURCES)

crawlerDriver: src/driver/driver.cpp
	make build
	$(CXX) $(CXXFLAGS) -O3 src/driver/driver.cpp -o $(BUILD_PATH)/driver.exe

# indexerDriver: src/indexer/driver.cpp src/indexer/index.cpp
# 	make build
# 	$(CXX) $(CXXFLAGS) src/indexer/driver.cpp src/indexer/index.cpp $(INCLUDES) -ltls -O3 -o $(BUILD_PATH)/indexerDriver.exe

# driverFinal: src/driver/driver.cpp
# 	make build
# 	$(CXX) $(CXXFLAGS) src/driver/driver.cpp $(INCLUDES) -ltls -O3 $(BUILD_PATH)/driver.exe

# build:
# 	@mkdir -vp $(addprefix $(BUILD_PATH)/tst/,$(TESTS_PATHS))

printOS:
	$(info Your OS is '$(UNAME_S)')

# TODO: run_integration_tests #

clean:
	@rm -rf $(BUILD_DIR)/

cleanDriver:
	@rm -rf data/tmp/logs/
	@rm -rf data/tmp/performance/

.PHONY: all test noop clean cleanDriver

# Keep all of our object files aroud for future compilations
.PRECIOUS: $(PRECIOUS_OBJECTS)
