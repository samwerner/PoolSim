#pragma once

class Share {
public:
  Share(bool _network_share);
  bool is_network_share() const;
private:
  bool network_share;
};

bool operator==(const Share& lhs, const Share& rhs);
bool operator!=(const Share& lhs, const Share& rhs);
