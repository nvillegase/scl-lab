#include <memory>
#include <stdexcept>
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


class SecretShareAndDistribute : public scl::proto::Protocol {
  
 public:

  static std::unique_ptr<scl::proto::Protocol> Create(int party_id,
                                                      int t,
                                                      int n,
                                                      Mersenne61& output_share) {
    return std::make_unique<SecretShareAndDistribute>(party_id, t, n, output_share);
  }

  static std::unique_ptr<scl::proto::Protocol> Create(int party_id,
                                                      int t,
                                                      int n,
                                                      Mersenne61& output_share,
                                                      Mersenne61 value) {
    return std::make_unique<SecretShareAndDistribute>(party_id, t, n, output_share, value);
  }

  SecretShareAndDistribute(size_t party_id,
                           int t,
                           int n,
                           Mersenne61& output_share)
                         : m_sender_party_id(party_id),
                           m_t(t),
                           m_n(n),
                           m_output_share(output_share),
                           m_value(Mersenne61(0)) {}

  SecretShareAndDistribute(size_t party_id,
                           int t,
                           int n,
                           Mersenne61& output_share,
                           Mersenne61 value)
                         : m_sender_party_id(party_id),
                           m_t(t),
                           m_n(n),
                           m_output_share(output_share),
                           m_value(value) {}

  std::unique_ptr<scl::proto::Protocol> Run(scl::proto::Env& env) override {

    auto local_party_id = env.network.MyId();

    auto prg = util::PRG::Create(std::to_string(local_party_id));
    scl::net::Packet pkt_send;
    
    if (local_party_id == m_sender_party_id) {

      auto x = m_value;
      if (x == Mersenne61(0)) {
        x = Mersenne61::Random(prg);
        cout << "- Sampled random x: 0x" << x << endl;
      }
      cout << "- Distributing shamir shares of 0x" << x << ":" << endl;

      const auto x_shares_t = ss::ShamirShare(x, m_t, m_n, prg);

      for (int i = 0; i < m_n; ++i) {
        pkt_send << x_shares_t[i];
        env.network.Party(i)->Send(pkt_send);
        cout << "\tSending 0x" << x_shares_t[i] << " to party " << i << endl;
        pkt_send.ResetWritePtr();
      }

    }

    cout << "- Receiving value from party " << m_sender_party_id << ": ";

    // Receive the share:
    auto pkt_recv = env.network.Party(m_sender_party_id)->Recv();
    if (!pkt_recv.has_value()) {
      throw std::logic_error("Didn't receive any value");
    }
    auto& pkt = pkt_recv.value();
    m_output_share = pkt.Read<Mersenne61>();

    cout << "0x" << m_output_share << endl;

    return nullptr;

  }

 private:
  size_t m_sender_party_id;
  const int m_t;
  const int m_n;
  Mersenne61 m_share_t;
  Mersenne61& m_output_share;
  Mersenne61 m_value;

};
