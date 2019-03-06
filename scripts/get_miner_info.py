"""
script to retrieve data about a single miner from the simulation results

usage: python get_miner_info.py /path/to/results MINER_ADDRESS
"""

import argparse
import gzip
import json


def open_file(filepath):
    if filepath.endswith(".gz"):
        return gzip.open(filepath, "rt")
    else:
        return open(filepath)


def read_data(filepath):
    with open_file(filepath) as f:
        return json.load(f)


def aggregate_results(data, address):
    miner = [v for v in data["miners"] if v["address"] == address]
    if not miner:
        raise ValueError("miner {0} does not exist".format(address))
    miner = miner[0]
    pool_data = [m for v in data["pools"] for m in v["miners"] if m["address"] == address]
    if not pool_data:
        raise ValueError("miner not in any pool")
    blocks_received = sum(v["metadata"]["blocks_received"] for v in pool_data)
    blocks_mined = sum(v["metadata"]["blocks_mined"] for v in pool_data)
    return dict(
        blocks_mined=blocks_mined,
        blocks_received=blocks_received,
        blocks_ratio=blocks_received / blocks_mined,
        work_per_block=miner["total_work"] / blocks_received
    )


def main():
    parser = argparse.ArgumentParser(prog="get-miner-info")
    parser.add_argument("results-file", help="results-file")
    parser.add_argument("address", help="miner address")
    args = vars(parser.parse_args())
    data = read_data(args["results-file"])
    print(json.dumps(aggregate_results(data, args["address"])))


if __name__ == "__main__":
    main()
