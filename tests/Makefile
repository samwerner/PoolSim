SRCS := $(wildcard *.cpp)
TESTS := $(patsubst %_test.cpp,build/%_test,$(SRCS))
RUN_TESTS := $(addsuffix .run, $(TESTS))

CXXFLAGS := -std=c++11 -Wall -g -I../libpoolsim -I../vendor -Wl,-rpath,$(realpath ../build) -L../build
LDFLAGS := -pthread -lgtest -lgmock -lpoolsim

all: test

build/%_test: %_test.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

build/%_test.run: build/%_test
	./$^

test: $(RUN_TESTS)

clean:
	rm -f $(TESTS)

.PHONY: clean
.SECONDARY: $(TESTS)
