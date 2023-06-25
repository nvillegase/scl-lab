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


class BGWProtocol : public scl::proto::Protocol {
  
 public:

  static std::unique_ptr<scl::proto::Protocol> Create(int t,
                                                      int n,
                                                      Mersenne61 x_share_t,
                                                      Mersenne61 y_share_t,
                                                      Mersenne61& output_xy_t) {
    return std::make_unique<BGWProtocol>(t, n, x_share_t, y_share_t, output_xy_t);
  }

  BGWProtocol(int t,
              int n,
              Mersenne61 x_share_t,
              Mersenne61 y_share_t,
              Mersenne61& output_xy_t)
            : m_t(t),
              m_n(n),
              m_x_share_t(x_share_t),
              m_y_share_t(y_share_t),
              m_output_xy_t(output_xy_t) {}

  std::unique_ptr<scl::proto::Protocol> Run(scl::proto::Env& env) override {

    auto local_party_id = env.network.MyId();
    auto prg = util::PRG::Create(std::to_string(local_party_id));
    scl::net::Packet pkt_send;
    
    auto xy_share_2t = m_x_share_t * m_y_share_t;
    cout << "- Multiplied 0x" << m_x_share_t << " and 0x" << m_y_share_t << " together to obtain ";
    cout << "xy_share_2t = 0x" << xy_share_2t << endl;
    cout << "- Resharing xy_share_2t... ";

    auto zi_shares = ss::ShamirShare(xy_share_2t, m_t, m_n, prg);

    cout << "sharings generated." << endl;

    // Shares of zi are distributed to all other parties:
    for (int i = 0; i < m_n; ++i) {
      pkt_send << zi_shares[i];
      env.network.Party(i)->Send(pkt_send);
      cout << "\tSending 0x" << zi_shares[i] << " to party " << i << endl;
      pkt_send.ResetWritePtr();
    }

    cout << "- Receiving shares from the other parties..." << endl;

    // Shares from other parties are received:
    auto zi_shares_recv = math::Vec<Mersenne61>(m_n);
    
    for (int i = 0; i < m_n; ++i) {
      auto pkt_recv_i = env.network.Party(i)->Recv();
      if (!pkt_recv_i.has_value()) {
        throw std::logic_error("Didn't receive any value");
      }
      auto& pkt_i = pkt_recv_i.value();
      zi_shares_recv[i] = pkt_i.Read<Mersenne61>();
      cout << "\tReceived 0x" << zi_shares_recv[i] << " from party " << i << endl;
    }

    cout << "- Recovering Shamir secret-shared value of degree " << m_t << " from " << m_n << " shares..." << endl;
    m_output_xy_t = ss::ShamirRecoverP(zi_shares_recv);

    cout << "- xy_share_t = 0x" << m_output_xy_t << endl;

    return nullptr;

  }

 private:
  const int m_t;
  const int m_n;
  Mersenne61 m_x_share_t;
  Mersenne61 m_y_share_t;
  Mersenne61& m_output_xy_t;

};
