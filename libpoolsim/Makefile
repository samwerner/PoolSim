SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:%.cpp=build/%.o)

LIBPOOLSIM := ../build/libpoolsim.so

CXX := g++
CXXFLAGS := -std=c++11 -Wall -g -MMD -MP -I../vendor -fPIC
LDFLAGS := -pthread

all: build_dir $(LIBPOOLSIM)

build_dir:
	@mkdir -p build ../build

$(LIBPOOLSIM): $(OBJS)
	$(CXX) -shared $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJS): build/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(OBJS:.o=.d)

clean:
	rm -f $(LIBPOOLSIM) $(OBJS) $(OBJS:.o=.d)

.PHONY: clean
