#include <scl/net/config.h>
#include <scl/net/network.h>
#include <scl/net/tcp_channel.h>
#include <scl/protocol/base.h>
#include <scl/util/cmdline.h>

#include "party.h"

using namespace scl;

void RealNetworkExecution(const util::ProgramOptions& opts) {

  // Get the ID of this party and the network config filename that was passed to
  // the program on the commandline.
  auto id = opts.Get<std::size_t>("id");
  auto conf = opts.Get("conf");

  // Create a NetworkConfig object from the file.
  auto network_conf = net::NetworkConfig::Load(id, std::string(conf));

  // Create a network. This takes care of connecting all the parties to each
  // other, using the information in the network config.
  auto network = net::Network::Create<net::TcpChannel<>>(network_conf);

  // This lambda will be called whenever our protocol generates any output.
  auto output_cb = [](std::any output) {
    auto received_msgs = std::any_cast<std::vector<int>>(output);
    for (const auto& m : received_msgs) {
      std::cout << "received " << m << "\n";
    }
  };

  // Evaluate the protocol for 5 rounds.
  proto::Evaluate(Party::Create((int)id, 5), network, output_cb);
}

int main(int argc, char** argv) {
  // This adds some command line arguments to our program, of which there are
  // only two :)
  const auto opts =
      util::ProgramOptions::Parser{}
          .Add(util::ProgramArg::Required("id", "int", "ID of this party"))
          .Add(util::ProgramArg::Required("conf", "string", "network config"))
          .Parse(argc, argv);

  RealNetworkExecution(opts);
}
