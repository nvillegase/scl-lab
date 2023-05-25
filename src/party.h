#ifndef EXAMPLE_PARTY_H
#define EXAMPLE_PARTY_H

#include <memory>
#include <stdexcept>
#include <utility>

#include <scl/protocol/base.h>
#include <scl/protocol/protocol.h>

/**
 * Very simple protocol where each party is started with a message M, and in
 * each round.
 *  - Sends their message plus a counter to the next party (in order of their
 *    IDs).
 *  - Receives the message from the previous party.
 *
 * For example, if we start three parties with messages 1, 2, and 3,
 * respectively, and run the protocol for 2 rounds, then the execution looks as
 * follows
 *
 * Round 1:
 *  - P_1 Sends 1 to P_2
 *  - P_2 Sends 2 to P_3
 *  - P_3 Sends 3 to P_1
 *  - P_1 Receives 3 from P_3
 *  - P_2 Receives 1 from P_1
 *  - P_3 Receives 2 from P_2
 *
 * Round 2:
 *  - P_1 Sends 2 to P_2
 *  - P_2 Sends 3 to P_3
 *  - P_3 Sends 4 to P_1
 *  - P_1 Receives 4 from P_3
 *  - P_2 Receives 2 from P_1
 *  - P_3 Receives 3 from P_2
 *
 * After the last round, each party outputs the list of messages received.
 */
class Party final : public scl::proto::Protocol {
 public:
  /**
   * Helper method for creating the first round of the protocol.
   */
  static std::unique_ptr<scl::proto::Protocol> Create(int message, int rounds) {
    std::vector<int> v;
    return std::make_unique<Party>(message, rounds, v);
  }

  /**
   * Constructor. Used by the Run function to initiate the next round of the
   * protocol.
   *
   * @param my_message the message to send in this round.
   * @param rounds_left how many rounds are remaining.
   * @param received_messages messages received so far.
   */
  Party(int my_message,
        int rounds_left,
        const std::vector<int>& received_messages)
      : m_my_message(my_message),
        m_rounds_left(rounds_left),
        m_received_messages(received_messages) {}

  /**
   * The method responsible for execution the logic of this protocol round.
   */
  std::unique_ptr<scl::proto::Protocol> Run(scl::proto::Env& env) override {
    // Create a Packet for sending, and add our message to it.
    scl::net::Packet pkt_send;
    pkt_send << m_my_message;

    // Send the packet to the next party.
    env.network.Next()->Send(pkt_send);

    // Attempt to receive the packet from the previous party. Receiving is
    // blocking by default, so if it fails, it means the other party crashed.
    auto pkt_recv = env.network.Previous()->Recv();
    if (!pkt_recv.has_value()) {
      throw std::logic_error("other party did not send a message");
    }

    // Packets obtained from the above receive call are of the type
    // std::optional<scl::net::Packet> so it needs to be unpacked first.
    auto& pkt = pkt_recv.value();

    // Add the message we receive to our list of messages.
    const auto received_message = pkt.Read<int>();
    m_received_messages.emplace_back(received_message);

    // If there's no more rounds left, then we stop (by returning a nullptr),
    // and set the output to be the messages we received during the protocol
    // execution.
    if (m_rounds_left == 0) {
      m_output = m_received_messages;
      return nullptr;
    }

    // Otherwise we return a new instance of our protocol were we increment our
    // message with 1, and decrement the number of rounds left by 1.
    return std::make_unique<Party>(m_my_message + 1,
                                   m_rounds_left - 1,
                                   m_received_messages);
  }

  /**
   * This method returns the protocol output, if any. The default value if an
   * std::any is an "empty" value, so we only get an actual output when the
   * protocol terminates.
   */
  std::any Output() const override {
    return m_output;
  }

 private:
  int m_my_message;
  int m_rounds_left;
  std::vector<int> m_received_messages;

  std::any m_output;
};

#endif  // EXAMPLE_PARTY_H
