SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:%.cpp=build/%.o)

LDFLAGS += -lpoolsim

all: build_dir $(POOLSIM)

build_dir:
	@mkdir -p build ../build

install:
	install -d $(PREFIX)/bin
	install -m 755 $(POOLSIM) $(PREFIX)/bin/$(notdir $(POOLSIM))

uninstall:
	rm -f $(PREFIX)/bin/$(notdir $(POOLSIM))

$(POOLSIM): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

build/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(POOLSIM) $(OBJS) $(OBJS:.o=.d)

.PHONY: clean
