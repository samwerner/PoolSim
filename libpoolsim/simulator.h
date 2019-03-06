#pragma once

#include <string>
#include <vector>
#include <map>

#include "miner.h"
#include "mining_pool.h"
#include "simulation.h"
#include "event_queue.h"
#include "event.h"
#include "random.h"

#include "miner_creator.h"
#include "observer.h"
#include "block_event.h"

namespace poolsim {

class Simulator :  public std::enable_shared_from_this<Simulator>,
                   public Observer<BlockEvent> {
public:
    explicit Simulator(Simulation simulation);
    Simulator(Simulation simulation, std::shared_ptr<Random> random);
    static std::shared_ptr<Simulator> from_config_file(const std::string& filepath);

    // Runs the simulator
    void run();

    // Initializes the simulator
    // creates pools and miners
    void initialize();

    // Saves the simulation data to a file
    void save_simulation_data();

    // Schedules all the miners
    // This should only be used for the first initialization
    void schedule_all();

    // Processes an event from the queue
    // This will also schedule the next event for the miner
    void process_event(const Event& event);

    // Schedules a single miner
    void schedule_miner(const std::shared_ptr<Miner> miner);

    // Adds a miner to the simulator
    void add_miner(std::shared_ptr<Miner> miner);

    // Adds a pool to the simulator
    void add_pool(std::shared_ptr<MiningPool> pool);

    // Returns the miner with the given address:
    std::shared_ptr<Miner> get_miner(const std::string& miner_address);

    // Returns the current number of blocks mined
    uint64_t get_blocks_mined() const;

    // Returns the current time
    double get_current_time() const;

    // Returns the numbers of pool
    size_t get_pools_count() const;

    // Returns the numbers of miners
    size_t get_miners_count() const;

    // Returns the numbers of scheduled events
    size_t get_events_count() const;

    // Returns the network instance
    std::shared_ptr<Network> get_network() const;

    // Returns the next event
    Event get_next_event() const;

    void process(const BlockEvent& block_event);

private:
    // Setup of the simulation to run
    Simulation simulation;

    // List of block events
    std::vector<BlockEvent> block_events;

    // Information about the network
    std::shared_ptr<Network> network;

    // Interface able to return random numbers
    // used mostly for testing purposes
    std::shared_ptr<Random> random;

    // Event queue of the simulator
    EventQueue queue;

    // Pools in the current simulation
    std::vector<std::shared_ptr<MiningPool>> pools;

    // Miners in the current simulation
    std::map<std::string, std::shared_ptr<Miner>> miners;

    int64_t duration;

    // Current time
    double current_time;

    // Total number of blocks mined
    uint64_t blocks_mined;

    // Outputs the result to a file
    void output_result(const nlohmann::json& result) const;
};

}
