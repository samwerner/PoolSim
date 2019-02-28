export POOLSIM := $(PWD)/build/poolsim
export LIBPOOLSIM := $(PWD)/build/libpoolsim.so
export CXX := g++
export CXXFLAGS := -std=c++11 -Wall -g -fPIC -I$(PWD)/libpoolsim -I$(PWD)/vendor -L$(PWD)/build -Wl,-rpath,$(PWD)/build
export LDFLAGS := -pthread
export PREFIX := $(PWD)/dist

all: $(POOLSIM)

$(POOLSIM): $(LIBPOOLSIM)
	$(MAKE) -C poolsim

$(LIBPOOLSIM): deps
	$(MAKE) -C libpoolsim

install:
	$(MAKE) -C libpoolsim install
	$(MAKE) -C poolsim install

uninstall:
	$(MAKE) -C libpoolsim uninstall
	$(MAKE) -C poolsim uninstall

deps:
	$(MAKE) -C vendor

test:  $(LIBPOOLSIM)
	$(MAKE) -C tests

clean_deps:
	rm -rf $(DEPS)

clean:
	$(MAKE) clean -C libpoolsim
	$(MAKE) clean -C poolsim
	$(MAKE) clean -C tests

distclean: clean
	$(MAKE) clean -C vendor

.PHONY: clean
