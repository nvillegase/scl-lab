#include <utility>
#include <iostream>

#include <scl/protocol/base.h>
#include <scl/protocol/protocol.h>

#include <scl/math/fp.h>
#include <scl/ss/shamir.h>
#include <scl/util/prg.h>

using namespace scl;
using namespace std;
using Mersenne61 = math::Fp<61>;


class SampleAndDistributeShares : public scl::proto::Protocol {
  
 public:

  static std::unique_ptr<scl::proto::Protocol> Create(int t, int n) {
    std::vector<int> v;
    return std::make_unique<SampleAndDistributeShares>(t, n);
  }

  SampleAndDistributeShares(int t, int n): m_t(t), m_n(n) {}

  std::unique_ptr<scl::proto::Protocol> Run(scl::proto::Env& env) override {

    auto prg = util::PRG::Create("prg-seed");
    
    auto x = Mersenne61::Random(prg);
    cout << "Sampled x = " << x << endl;

    const auto x_shares_t = ss::ShamirShare(x, m_t, m_n, prg);

    for (int i = 0; i < m_n; ++i) {
      scl::net::Packet pkt_send;
      pkt_send << x_shares_t[i];
      env.network.Party(i)->Send(pkt_send);
      cout << "Sending " << x_shares_t[i] << " to party " << i << endl;
    }

    return nullptr;

  }

 private:
  const int m_t;
  const int m_n;

};
