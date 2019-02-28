SRCS := $(wildcard *.cpp)
HEADERS := $(wildcard *.h)
OBJS := $(SRCS:%.cpp=build/%.o)

ifeq ($(RELEASE),false)
	CXXFLAGS += -MMD -MP
endif

all: build_dir $(LIBPOOLSIM)

build_dir:
	@mkdir -p build ../build

$(LIBPOOLSIM): $(OBJS)
	$(CXX) -shared $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJS): build/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

install:
	install -d $(PREFIX)/lib
	install -m 755 $(LIBPOOLSIM) $(PREFIX)/lib/$(notdir $(LIBPOOLSIM))
	install -d $(PREFIX)/include/poolsim
	install -m 644 $(HEADERS) $(PREFIX)/include/poolsim

uninstall:
	rm -f $(PREFIX)/lib/$(notdir $(LIBPOOLSIM))
	rm -rf $(PREFIX)/include/poolsim

-include $(OBJS:.o=.d)

clean:
	rm -f $(LIBPOOLSIM) $(OBJS) $(OBJS:.o=.d)

.PHONY: clean
