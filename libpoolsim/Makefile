SRCS := $(wildcard *.cpp)
HEADERS := $(wildcard *.h)
OBJS := $(SRCS:%.cpp=build/%.o)

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
	install -d $(PREFIX)/include/libpoolsim
	install -m 644 $(HEADERS) $(PREFIX)/include/libpoolsim

uninstall:
	rm -f $(PREFIX)/lib/$(notdir $(LIBPOOLSIM))
	rm -rf $(PREFIX)/include/libpoolsim

-include $(OBJS:.o=.d)

clean:
	rm -f $(LIBPOOLSIM) $(OBJS) $(OBJS:.o=.d)

.PHONY: clean
