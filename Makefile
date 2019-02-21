SRCS = $(wildcard src/*.cpp)
OBJ = $(SRCS:src/%.cpp=build/%.o)

CC_VENDOR_SRCS = $(wildcard vendor/*/*.cc)
CC_VENDOR_OBJ = $(CC_VENDOR_SRCS:vendor/%.cc=build/%.o)

ALL_OBJS = $(OBJ) $(CC_VENDOR_OBJ)

DEPS = vendor/nlohmann vendor/easyloggingpp
EASYLOGGING_VERSION = 9.96.7

EXE = build/poolsim

CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -MMD -I$(PWD)/vendor
LDFLAGS = -lboost_program_options


all: build_dir deps $(EXE)

build_dir:
	@mkdir -p build

$(EXE): $(ALL_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(CC_VENDOR_OBJ): build/%.o: vendor/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(OBJ:.o=.d)

clean_deps:
	rm -rf $(DEPS)

clean:
	rm -rf $(dir $(CC_VENDOR_OBJ))
	rm -f $(EXE) $(OBJ) $(OBJ:.o=.d)

distclean: clean clean_deps
	rm -rf build
	rm -rf vendor

.PHONY: clean

deps: $(DEPS)

vendor/easyloggingpp: vendor/easyloggingpp/easylogging++.cc
vendor/easyloggingpp/easylogging++.cc:
	@mkdir -p vendor/easyloggingpp
	@curl -L https://github.com/zuhd-org/easyloggingpp/archive/v$(EASYLOGGING_VERSION).tar.gz | \
		tar xvz -C vendor/easyloggingpp easyloggingpp-$(EASYLOGGING_VERSION)/src --strip-components=2

vendor/nlohmann: vendor/nlohmann/json.hpp
vendor/nlohmann/json.hpp:
	@mkdir -p vendor/nlohmann
	@wget https://github.com/nlohmann/json/releases/download/v3.5.0/json.hpp -O $@
