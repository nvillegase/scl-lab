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


class ShamirReconstructSq : public scl::proto::Protocol {
  
 public:

  static std::unique_ptr<scl::proto::Protocol> Create(Mersenne61 share_t,
                                                      int t,
                                                      int n,
                                                      Mersenne61& output) {
    return std::make_unique<ShamirReconstructSq>(share_t, t, n, output);
  }

  ShamirReconstructSq(Mersenne61 share_t,
                      int t,
                      int n,
                      Mersenne61& output)
                    : m_share_t(share_t),
                      m_t(t),
                      m_n(n),
                      m_output(output) {}

  std::unique_ptr<scl::proto::Protocol> Run(scl::proto::Env& env) override {

    scl::net::Packet pkt_send;
    pkt_send << m_share_t;

    cout << "- Initiating reconstruction of secret-shared value of degree " << m_t << "." << endl;
    
    for (int i = 0; i < m_n; ++i) {
      env.network.Party(i)->Send(pkt_send);
      cout << "\tSending 0x" << m_share_t << " to party " << i << endl;
    }

    auto shares = math::Vec<Mersenne61>(m_n);
    cout << "- Receiving shares from the other parties..." << endl;

    // Receive the other shares:
    for (int i = 0; i < m_n; ++i) {

      auto pkt_recv = env.network.Party(i)->Recv();
      
      if (!pkt_recv.has_value()) {
        throw std::logic_error("Didn't receive share");
      }
      auto& pkt = pkt_recv.value();
      shares[i] = pkt.Read<Mersenne61>();
      cout << "\tReceived 0x" << shares[i] << " from party " << i << endl;
    
    }

    cout << "- Recovering Shamir secret-shared value from " << m_n << " shares..." << endl;

    m_output = ss::ShamirRecoverP(shares);

    cout << "- Recovered value 0x" << m_output << endl;

    return nullptr;

  }

 private:
  Mersenne61 m_share_t;
  const int m_t;
  const int m_n;
  Mersenne61& m_output;

};
