#include <scl/net/config.h>
#include <scl/net/network.h>
#include <scl/net/tcp_channel.h>
#include <scl/protocol/base.h>
#include "scl/protocol/env.h"

#include "../utils.h"
#include "ssnd.cc"

using namespace scl;

extern "C" {
  uint64_t SSNDExec(int local_party_id, int sending_party_id, int n, int t, char* conf, uint64_t value);
}

uint64_t SSNDExec(int local_party_id, int sending_party_id, int n, int t, char* conf, uint64_t value) {

  if (2*t + 1 > n) {
    throw std::logic_error("Invalid threshold");
  }

  auto f_value = Mersenne61::FromString(int_to_hex(value));

  // Create a NetworkConfig object from the file.
  auto network_conf = net::NetworkConfig::Load(local_party_id, conf);
  auto network = net::Network::Create<net::TcpChannel<>>(network_conf);

  Mersenne61 output_share;
  proto::Evaluate(SecretShareAndDistribute::Create(sending_party_id, t, n, output_share, f_value), network);

  uint64_t int_value = std::stoull(output_share.ToString(), nullptr, 16);

  return int_value;

}
