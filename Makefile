POOLSIM := build/poolsim
LIBPOOLSIM := build/libpoolsim.so

all: $(POOLSIM)

$(POOLSIM): deps $(LIBPOOLSIM)
	$(MAKE) -C poolsim

$(LIBPOOLSIM):
	$(MAKE) -C libpoolsim

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
