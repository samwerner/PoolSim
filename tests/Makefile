SRCS := $(wildcard *.cpp)
TESTS := $(patsubst %_test.cpp,build/%_test,$(SRCS))
RUN_TESTS := $(addsuffix .run, $(TESTS))

LDFLAGS += -lgtest -lgmock -lpoolsim

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
