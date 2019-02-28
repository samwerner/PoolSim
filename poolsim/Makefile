SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:%.cpp=build/%.o)

POOLSIM := ../build/poolsim

CXX := g++
CXXFLAGS := -std=c++11 -Wall -g -MMD -MP -fPIC -I../libpoolsim -I../vendor -Wl,-rpath,$(realpath ../build) -L../build
LDFLAGS := -pthread -lpoolsim

all: build_dir $(POOLSIM)

build_dir:
	@mkdir -p build ../build

$(POOLSIM): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJS): build/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(POOLSIM) $(OBJS) $(OBJS:.o=.d)

.PHONY: clean
