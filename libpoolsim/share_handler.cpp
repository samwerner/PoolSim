#include "share_handler.h"
#include "miner.h"
#include "factory.h"


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


QBWithholdingShareHandler::QBWithholdingShareHandler(const nlohmann::json& _args) {}

void QBWithholdingShareHandler::handle_share(const Share& share) {
    // Check if condition is met 
    //if (!condition.is_true())
    //    get_miner()->get_pool()->submit_share(get_miner()->get_address(), share);
   // get_miner()->get_pool()->get
}

REGISTER(ShareHandler, QBWithholdingShareHandler, "qb_share_withholding")


DonationShareHandler::DonationShareHandler(const nlohmann::json& _args) {}

void DonationShareHandler::handle_share(const Share& share) {

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
