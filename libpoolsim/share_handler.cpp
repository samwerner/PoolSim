#include "share_handler.h"
#include "miner.h"
#include "factory.h"
#include "miner_record.h"
#include <iterator>

namespace poolsim {

void from_json(const nlohmann::json& j, BehaviourConfig& b) {
        if (j.find("top_n") != j.end())
            j.at("top_n").get_to(b.top_n);
        if (j.find("threshold") != j.end())
            j.at("threshold").get_to(b.threshold);
}

ShareHandler::~ShareHandler() {}

void ShareHandler::set_miner(std::shared_ptr<Miner> _miner) {
  miner = _miner;
}

std::shared_ptr<Miner> ShareHandler::get_miner() {
  return miner.lock();
}

// NOTE: this particular class probably does not need for args
// but it must accept them because of the current factory implementation
DefaultShareHandler::DefaultShareHandler(const nlohmann::json& _args) {}

void DefaultShareHandler::handle_share(const Share& share) {
  get_miner()->get_pool()->submit_share(get_miner()->get_address(), share);
}

REGISTER(ShareHandler, DefaultShareHandler, "default")

WithholdingShareHandler::WithholdingShareHandler(const nlohmann::json& _args) {}

void WithholdingShareHandler::handle_share(const Share& share) {
    if (!share.is_valid_block())
        get_miner()->get_pool()->submit_share(get_miner()->get_address(), share);
}

REGISTER(ShareHandler, WithholdingShareHandler, "share_withholding")

bool QBShareHandler::condition_true(std::vector<std::shared_ptr<QBRecord>>& records) {
    for (std::vector<std::shared_ptr<QBRecord>>::iterator record = records.begin(); record != records.end(); ++record) {
        if (std::distance(records.begin(), record) > top_n)
            return false;

        if ((*record)->get_miner() == this->get_miner()->get_address()) {
            uint64_t victim_index = std::distance(records.begin(), record)+1;
            uint64_t victim_credits = records[victim_index]->get_credits();
            victim_miner = records[victim_index]->get_miner();
            if ((*record)->get_credits()*threshold <= victim_credits) {
                return true;
            } 
        }
    }
    return false;
}

QBWithholdingShareHandler::QBWithholdingShareHandler(const nlohmann::json& _args) {
    BehaviourConfig config;
    from_json(_args, config);
    top_n = config.top_n;
    threshold = config.threshold;
}

void QBWithholdingShareHandler::handle_share(const Share& share) {
    if (!(get_miner()->get_pool()->get_name() == "qb")) {
        get_miner()->get_pool()->submit_share(get_miner()->get_address(), share);
        return;   
    }
    
    auto records = get_miner()->get_pool()->get_records<QBRewardScheme>();
    std::sort(records.begin(), records.end(), QBSortObj());
    if (condition_true(records)) {
        // Do nothing == withhold share
    } else
        get_miner()->get_pool()->submit_share(get_miner()->get_address(), share);
}

REGISTER(ShareHandler, QBWithholdingShareHandler, "qb_share_withholding")


DonationShareHandler::DonationShareHandler(const nlohmann::json& _args) {
    BehaviourConfig config;
    from_json(_args, config);
    top_n = config.top_n;
    threshold = config.threshold;
}

void DonationShareHandler::handle_share(const Share& share) {
    if (!(get_miner()->get_pool()->get_name() == "qb")) {
        get_miner()->get_pool()->submit_share(get_miner()->get_address(), share);
        return;   
    }
    
    auto records = get_miner()->get_pool()->get_records<QBRewardScheme>();
    std::sort(records.begin(), records.end(), QBSortObj());
    if (condition_true(records)) {
        get_miner()->get_pool()->submit_share(victim_miner, share);
    } else
        get_miner()->get_pool()->submit_share(get_miner()->get_address(), share);
}

REGISTER(ShareHandler, DonationShareHandler, "share_donation")


SecondWalletShareHandler::SecondWalletShareHandler(const nlohmann::json& _args) {

}

void SecondWalletShareHandler::handle_share(const Share& share) {

}

REGISTER(ShareHandler, SecondWalletShareHandler, "second_wallet")


MultipleAddressesShareHandler::MultipleAddressesShareHandler(const nlohmann::json& _args) {}

void MultipleAddressesShareHandler::handle_share(const Share& share) {

}

REGISTER(ShareHandler, MultipleAddressesShareHandler, "multiple_addresses");

}
