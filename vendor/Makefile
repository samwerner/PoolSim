DEPS = nlohmann spdlog csv.h CLI11.hpp
SPDLOG_VERSION = 1.3.1

deps: $(DEPS)

csv.h:
	@wget https://raw.githubusercontent.com/ben-strasser/fast-cpp-csv-parser/master/csv.h -O $@

CLI11.hpp:
	@wget https://github.com/CLIUtils/CLI11/releases/download/v1.7.1/CLI11.hpp -O $@

spdlog: spdlog/spdlog.h
spdlog/spdlog.h:
	@mkdir -p spdlog
	@curl -L https://github.com/gabime/spdlog/archive/v$(SPDLOG_VERSION).tar.gz | \
		tar xvz spdlog-$(SPDLOG_VERSION)/include --strip-components=2

nlohmann: nlohmann/json.hpp
nlohmann/json.hpp:
	@mkdir -p nlohmann
	@wget https://github.com/nlohmann/json/releases/download/v3.5.0/json.hpp -O $@

clean:
	rm -rf $(DEPS)
