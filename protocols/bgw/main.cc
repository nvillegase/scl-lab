#include <scl/net/config.h>
#include <scl/net/network.h>
#include <scl/net/tcp_channel.h>
#include <scl/protocol/base.h>
#include "scl/protocol/env.h"
#include <scl/math/fp.h>

#include "../utils.h"
#include "bgw.cc"


extern "C" {
  uint64_t BGWExec(int local_party_id, int n, int t, char* conf, uint64_t x_share_t, uint64_t y_share_t);
}


uint64_t BGWExec(int local_party_id, int n, int t, char* conf, uint64_t x_share_t, uint64_t y_share_t) {

  if (2*t + 1 > n) {
    throw std::logic_error("Invalid threshold");
  }

  auto f_x_share_t = Mersenne61::FromString(int_to_hex(x_share_t));
  auto f_y_share_t = Mersenne61::FromString(int_to_hex(y_share_t));

  // Create a NetworkConfig object from the file.
  auto network_conf = scl::net::NetworkConfig::Load(local_party_id, conf);
  auto network = scl::net::Network::Create<net::TcpChannel<>>(network_conf);

  Mersenne61 output_share;
  proto::Evaluate(BGWProtocol::Create(t, n, f_x_share_t, f_y_share_t, output_share), network);

  uint64_t int_value = std::stoull(output_share.ToString(), nullptr, 16);

  return int_value;

}
