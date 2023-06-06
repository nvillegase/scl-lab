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

  static std::unique_ptr<scl::proto::Protocol> Create(int party_id, int t, int n) {
    std::vector<int> v;
    return std::make_unique<BGWProtocol>(party_id, t, n);
  }

  BGWProtocol(int party_id,
              int t,
              int n)
            : m_party_id(party_id),
              m_t(t),
              m_n(n) {}

  std::unique_ptr<scl::proto::Protocol> Run(scl::proto::Env& env) override {

    auto prg = util::PRG::Create(std::to_string(m_party_id));
    
    if (m_party_id == 0) {

      auto x = Mersenne61::Random(prg);
      cout << "Sampled x = " << x << endl;

      const auto x_shares_t = ss::ShamirShare(x, m_t, m_n, prg);

      for (int i = 0; i < m_n; ++i) {
        scl::net::Packet pkt_send;
        pkt_send << x_shares_t[i];
        env.network.Party(i)->Send(pkt_send);
        cout << "Sending " << x_shares_t[i] << " to party " << i << endl;
      }

    }
    else if (m_party_id == 1) {

      auto y = Mersenne61::Random(prg);
      cout << "Sampled y = " << y << endl;

      const auto y_shares_t = ss::ShamirShare(y, m_t, m_n, prg);

      for (int i = 0; i < m_n; ++i) {
        scl::net::Packet pkt_send;
        pkt_send << y_shares_t[i];
        env.network.Party(i)->Send(pkt_send);
        cout << "Sending " << y_shares_t[i] << " to party " << i << endl;
      }

    }

    // Receive the share of X:
    auto pkt_recv_0 = env.network.Party(0)->Recv();
    if (!pkt_recv_0.has_value()) {
      throw std::logic_error("Party 0 didn't send the share of X");
    }
    auto& pkt_0 = pkt_recv_0.value();
    auto x_share_t = pkt_0.Read<Mersenne61>();
    cout << "Received share of x: " << x_share_t << endl;

    // Receive the share of Y:
    auto pkt_recv_1 = env.network.Party(1)->Recv();
    if (!pkt_recv_1.has_value()) {
      throw std::logic_error("Party 1 didn't send the share of Y");
    }
    auto& pkt_1 = pkt_recv_1.value();
    auto y_share_t = pkt_1.Read<Mersenne61>();
    cout << "Received share of y: " << y_share_t << endl;

    auto xy_share_2t = x_share_t * y_share_t;
    cout << "xy_share_2t = " << xy_share_2t << endl;

    auto zi_shares = ss::ShamirShare(xy_share_2t, m_t, m_n, prg);

    // Shares of zi are distributed to all other parties:
    for (int i = 0; i < m_n; ++i) {
      scl::net::Packet pkt_send;
      pkt_send << zi_shares[i];
      env.network.Party(i)->Send(pkt_send);
      cout << "Sending " << zi_shares[i] << " to party " << i << endl;
    }

    // Shares from other parties are received:
    auto zi_shares_recv = math::Vec<Mersenne61>(m_n);
    
    for (int i = 0; i < m_n; ++i) {
      auto pkt_recv_i = env.network.Party(i)->Recv();
      if (!pkt_recv_i.has_value()) {
        // pass
      }
      auto& pkt_i = pkt_recv_i.value();
      zi_shares_recv[i] = pkt_i.Read<Mersenne61>();
    }

    auto xy_share_t = ss::ShamirRecoverP(zi_shares_recv);

    cout << "xy_share_t = " << xy_share_t << endl;

    // For reconstruction of xy, send to party 0:
    scl::net::Packet pkt_send;
    pkt_send << xy_share_t;
    env.network.Party(0)->Send(pkt_send);
    
    auto xy_shares_t = math::Vec<Mersenne61>(m_n);

    if (m_party_id == 0) {
      // Receive xy_share_t from all other parties and reconstruct:
      for (int i = 0; i < m_n; ++i) {
        auto pkt_recv_i = env.network.Party(i)->Recv();
        if (!pkt_recv_i.has_value()) {
          // pass
        }
        auto& pkt_i = pkt_recv_i.value();
        xy_shares_t[i] = pkt_i.Read<Mersenne61>();
      }
      auto xy = ss::ShamirRecoverP(xy_shares_t);

      // Then, share the result with the other parties:
      for (int i = 0; i < m_n; ++i) {
        scl::net::Packet pkt_send;
        pkt_send << xy;
        env.network.Party(i)->Send(pkt_send);
      }
    }

    auto pkt_recv = env.network.Party(0)->Recv();
    if (!pkt_recv.has_value()) {
      // pass
    }
    auto& pkt = pkt_recv.value();
    m_xy = pkt.Read<Mersenne61>();

    return nullptr;

  }

  std::any Output() const override {
    return m_xy;
  }

 private:
  const int m_party_id;
  const int m_t;
  const int m_n;
  Mersenne61 m_xy;
  Mersenne61 m_share_x;
  Mersenne61 m_share_y;

};
