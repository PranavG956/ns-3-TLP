// #include "ns3/test.h"
// #include "ns3/tcp-socket-base.h"
// #include "ns3/node.h"
// #include "ns3/simulator.h"
// #include "ns3/error-model.h"
// #include "ns3/tcp-header.h"
// #include "ns3/ipv4-header.h"

// using namespace ns3;

// class TcpTlpRecoveryTest : public TestCase
// {
// public:
//     TcpTlpRecoveryTest();
    
//     void PacketDropped(Ptr<const Packet> p);
//     void TlpProbeAcked(SequenceNumber32 probeSeq, SequenceNumber32 ackNum);

// private:
//     virtual void DoRun(void);
    
//     uint32_t m_droppedPackets;
//     uint32_t m_tlpAckEvents;
// };

// TcpTlpRecoveryTest::TcpTlpRecoveryTest()
//     : TestCase("Test TLP tail loss recovery"),
//       m_droppedPackets(0),
//       m_tlpAckEvents(0)
// {
// }

// void
// TcpTlpRecoveryTest::PacketDropped(Ptr<const Packet> p)
// {
//     NS_LOG_INFO("Packet dropped");
//     m_droppedPackets++;
// }

// void
// TcpTlpRecoveryTest::TlpProbeAcked(SequenceNumber32 probeSeq, SequenceNumber32 ackNum)
// {
//     NS_LOG_INFO("TLP probe ACKed: probe=" << probeSeq << " ack=" << ackNum);
//     m_tlpAckEvents++;
// }

// void
// TcpTlpRecoveryTest::DoRun()
// {
//     // Create a simple topology with loss
//     Ptr<Node> node1 = CreateObject<Node>();
//     Ptr<Node> node2 = CreateObject<Node>();
    
//     InternetStackHelper internet;
//     internet.Install(node1);
//     internet.Install(node2);
    
//     SimpleNetDeviceHelper helper;
//     helper.SetNetDevicePointToPointMode(true);
//     NetDeviceContainer devices = helper.Install(node1, node2);
    
//     // Add error model to drop tail packets
//     Ptr<RateErrorModel> em = CreateObject<RateErrorModel>();
//     em->SetUnit(RateErrorModel::ERROR_UNIT_PACKET);
//     em->SetRate(0.3); // 30% packet loss
//     devices.Get(0)->SetAttribute("ReceiveErrorModel", PointerValue(em));
    
//     Ipv4AddressHelper ipv4;
//     ipv4.SetBase("10.1.1.0", "255.255.255.0");
//     Ipv4InterfaceContainer interfaces = ipv4.Assign(devices);
    
//     // Create TCP socket with TLP enabled
//     Ptr<TcpSocketBase> socket = Socket::CreateSocket(node1, TcpSocketFactory::GetTypeId());
//     socket->SetAttribute("TlpEnabled", BooleanValue(true));
//     socket->SetAttribute("TlpTimeoutMin", TimeValue(MilliSeconds(20)));
    
//     // Connect traces
//     socket->TraceConnectWithoutContext("TlpAck", MakeCallback(&TcpTlpRecoveryTest::TlpProbeAcked, this));
//     devices.Get(0)->TraceConnectWithoutContext("PhyRxDrop", MakeCallback(&TcpTlpRecoveryTest::PacketDropped, this));
    
//     // Bind and connect
//     socket->Bind();
//     socket->Connect(InetSocketAddress(interfaces.GetAddress(1), 9));
    
//     // Send multiple packets (some will be lost)
//     for (int i = 0; i < 10; i++)
//     {
//         Ptr<Packet> pkt = Create<Packet>(500);
//         socket->Send(pkt);
//     }
    
//     // Run long enough for TLP to detect losses and recover
//     Simulator::Stop(Seconds(1.0));
//     Simulator::Run();
    
//     // Verify recovery occurred
//     NS_TEST_ASSERT_MSG_GT(m_droppedPackets, 0, "Some packets should have been dropped");
//     NS_LOG_INFO("Packets dropped: " << m_droppedPackets << ", TLP ACK events: " << m_tlpAckEvents);
    
//     // TLP should have attempted recovery for tail losses
//     NS_TEST_ASSERT_MSG_GT(m_tlpAckEvents, 0, "TLP should have attempted recovery");
    
//     Simulator::Destroy();
// }