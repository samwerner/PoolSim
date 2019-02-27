# PoolSim

## Requirements

* C++11 compiler
* [Google Test][google-test] and Google Mock (only for tests, packages `google-mock` and `libgtest-dev` on Ubuntu)

NOTE: On Ubuntu, Google Mock and Google Test must be compiled.
To install them system-wide, you can use the following commands.

```
mkdir /usr/src/googletest/build
cd /usr/src/googletest/build
cmake ..
make
make install
```

## Build

Running

```
make
```

should build everything into the `build` directory.
Executable is at `build/poolsim`

## Running the simulator

Simulations need a configuration file to work. A sample config file
can be found at [config.json](./config.json).

The simulator can be run with the following command.

```
./build/poolsim --config config.json
```

You can try the `--help` flag for information about other optional flags.

## Development

### Implementing a new miner behavior

Miner behaviors are currently implemented as `ShareHandler` subclasses.
However, some of the required methods have already been implemented in the
templated `BaseShareHandler` class. To create `NewShareHandler`,
we therefore recommend to use to subclass `BaseShareHandler<NewShareHandler>`.

The `ShareHandler` subclasses constructor must accept a `const nlohmann::json&`
parameter, which is populated by the `pools[N].reward_scheme.params` key in the config file.
This can be used to dynamnically custom parameters from the config file.
The `REGISTER` macro must be called from the implementation file for the
class to be found from the name set in the config file. The first
parameter must stay `ShareHandler`, as this is the type we are trying to register.
The second parameter is the name of the new class. The third parameter
is the name to be used in the config file.

While `ShareHandler` has access to the miner, this should be mostly read only
and the potential state should be kept in the implementation of `ShareHandler`.

This is the minimum code to implement a new behavior is as follow.

```c++
// new_share_handler.h
#include <nlohmann/json.hpp>
#include "share_handler.h"

class NewShareHandler: public BaseShareHandler<NewShareHandler> {
public:
  explicit NewShareHandler(const nlohmann::json& args);
  void handle_share(const Share& share) override;
};

// new_share_handler.cpp
#include "new_share_handler.h"

NewShareHandler::NewShareHandler(const nlohmann::json& _args) {}

void NewShareHandler::handle_share(const Share& share) {
  // do greatness
}

REGISTER(ShareHandler, NewShareHandler, "some_name")
```

### Implementing a new reward scheme

Implementation-wise, reward schemes are very similar to miners' share handlers.
Reward schemes must subclass the `RewardScheme` base class and a `BaseRewardScheme`
is also provided to reduce boilerplate.

Below is the mininum code to create a new reward scheme.

```c++
// new_reward_scehme.h
#include <nlohmann/json.hpp>
#include "reward_scheme.h"

class NewRewardScheme: public BaseRewardScheme<NewRewardScheme> {
public:
  explicit NewRewardScheme(const nlohmann::json& args);
  void handle_share(const std::string& miner_address, const Share& share) override;
};

// new_reward_scheme.cpp
NewRewardScheme::NewRewardScheme(const nlohmann::json& _args) {}

void NewRewardScheme::handle_share(const std::string& miner_address, const Share& share) {
}

REGISTER(RewardScheme, NewRewardScheme, "some_name")
```

### Running the tests

Tests require Google Test and Google Mock.
Running

```
make test
```

should run and execute the tests.

## Progress

- [x] Simulator core logic
- [x] Config parser
- [x] Network difficulty
- [x] Pool
  - [x] Difficulty
  - [x] Uncle block prob
- [x] Miners
    - [x] From CSV
    - [x] From distribution
        - [x] Normal distribution
        - [x] Uniform distribution
- [ ] Result serialization
    - [ ] Decide what to save and where
- [ ] Reward schemes
    - [ ] PPS
    - [ ] PPLNS
    - [ ] QB
- [ ] Miner behaviors
    - [x] Default
    - [ ] Stop mining
    - [ ] Donate
- [ ] Multiple simulations per run
    - [ ] Config file scheme
    - [ ] Simulator implementation


[google-test]: https://github.com/google/googletest
