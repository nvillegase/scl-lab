#include <scl/net/config.h>
#include <scl/net/network.h>
#include <scl/net/tcp_channel.h>
#include <scl/protocol/base.h>
#include "scl/protocol/env.h"

#include "../utils.h"
#include "srsq.cc"

using namespace scl;

extern "C" {
  uint64_t SRSQExec(int local_party_id, uint64_t share, int t, int n, char* conf);
}

uint64_t SRSQExec(int local_party_id, uint64_t share, int t, int n, char* conf) {

  if (2*t + 1 > n) {
    throw std::logic_error("Invalid threshold");
  }

  // Create a NetworkConfig object from the file.
  auto network_conf = net::NetworkConfig::Load(local_party_id, conf);
  auto network = net::Network::Create<net::TcpChannel<>>(network_conf);

  auto f_share = Mersenne61::FromString(int_to_hex(share));

  Mersenne61 output;
  proto::Evaluate(ShamirReconstructSq::Create(f_share, t, n, output), network);

  uint64_t int_output = std::stoull(output.ToString(), nullptr, 16);

  return int_output;

}
