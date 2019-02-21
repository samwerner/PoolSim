#pragma once

class Share {
public:
  Share(bool _network_share);
  bool is_network_share() const;
private:
  bool network_share;
};
