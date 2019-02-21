SRCS = $(subst src/main.cpp,,$(wildcard src/*.cpp))
OBJ = $(SRCS:src/%.cpp=build/%.o)
MAIN_OBJ = src/main.o


TEST_SRCS = $(wildcard tests/*.cpp)
TESTS = $(patsubst tests/%_test.cpp,build/%_test,$(TEST_SRCS))
RUN_TESTS = $(addsuffix .run, $(TESTS))


TEST_OBJS = $(OBJ)
ALL_OBJS = $(TEST_OBJS) $(MAIN_OBJ)

DEPS = vendor/nlohmann vendor/spdlog
SPDLOG_VERSION = 1.3.1

EXE = build/poolsim

CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -MMD -I$(PWD)/vendor
LDFLAGS = -lboost_program_options -pthread
LDFLAGS_TEST = $(LDFLAGS) -lgtest -lgmock

all: build_dir deps $(EXE)

build_dir:
	@mkdir -p build

$(EXE): $(ALL_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

build/%_test: tests/%_test.cpp $(TEST_OBJS)
	$(CXX) $(CXX_FLAGS) -I$(PWD)/src $^ -o $@ $(LDFLAGS_TEST)

build/%_test.run: build/%_test
	./$^

test: $(RUN_TESTS)

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(OBJ:.o=.d)

clean_deps:
	rm -rf $(DEPS)

clean:
	rm -f $(EXE) $(OBJ) $(OBJ:.o=.d) $(TESTS)

distclean: clean clean_deps
	rm -rf build
	rm -rf vendor

.PHONY: clean
.SECONDARY: $(TESTS)

deps: $(DEPS)

vendor/spdlog: vendor/spdlog/spdlog.h
vendor/spdlog/spdlog.h:
	@mkdir -p vendor/spdlog
	@curl -L https://github.com/gabime/spdlog/archive/v$(SPDLOG_VERSION).tar.gz | \
		tar xvz -C vendor spdlog-$(SPDLOG_VERSION)/include --strip-components=2

vendor/nlohmann: vendor/nlohmann/json.hpp
vendor/nlohmann/json.hpp:
	@mkdir -p vendor/nlohmann
	@wget https://github.com/nlohmann/json/releases/download/v3.5.0/json.hpp -O $@
