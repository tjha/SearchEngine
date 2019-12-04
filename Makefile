CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

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
INCLUDES = -I $(SRC_PATH)/utils/ -I $(SRC_PATH)/parser/ -I $(SRC_PATH)/spinarak/ -I $(TEST_PATH) $(LDFLAGS) $(CPPFLAGS)

TEST_SOURCES := $(wildcard $(TEST_PATH)/*/*.cpp)
TESTS := $(patsubst $(TEST_PATH)/%Tests.cpp,$(BUILD_PATH)/tst/%Tests.exe,$(TEST_SOURCES))
TESTS_PATHS := $(filter-out $(TEST_PATH)/%.cpp $(TEST_PATH)/%.hpp, $(wildcard $(TEST_PATH)/*/))
TESTS_PATHS := $(patsubst $(TEST_PATH)/%,%,$(TESTS_PATHS))

MODULE_CASES := $(wildcard $(TEST_PATH)/$(module)/*.cpp)
MODULE_TESTS := $(patsubst $(TEST_PATH)/%Tests.cpp,$(BUILD_PATH)/tst/%Tests.exe,$(MODULE_CASES))

all: print_os $(TESTS)

driver: src/mvp/driver.cpp
	make build
	$(CXX) $(CXXFLAGS) src/mvp/driver.cpp $(INCLUDES) -ltls -pthread -o $(BUILD_PATH)/driver.exe

multithreadingTest: src/mvp/multithreadingTest.cpp
	$(CXX) $(CXXFLAGS) src/mvp/multithreadingTest.cpp $(INCLUDES) -ltls -pthread -o build/multithreadingTest.exe

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

