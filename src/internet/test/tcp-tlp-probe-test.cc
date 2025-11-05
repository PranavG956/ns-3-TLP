// #include "ns3/test.h"
// #include "ns3/tcp-socket-base.h"
// #include "ns3/node.h"
// #include "ns3/log.h"
// #include "ns3/simulator.h"
// #include "ns3/simple-net-device.h"
// #include "ns3/simple-channel.h"
// #include "ns3/internet-stack-helper.h"
// #include "ns3/ipv4-address-helper.h"
// #include "ns3/inet-socket-address.h"

// using namespace ns3;

// NS_LOG_COMPONENT_DEFINE("TcpTlpProbeTest");

// class TcpTlpProbeTest : public TestCase
// {
// public:
//     TcpTlpProbeTest();
    
//     // Callbacks for testing
//     void TlpProbeSent(SequenceNumber32 seq, uint32_t probeCount);

// private:
//     virtual void DoRun(void);
    
//     uint32_t m_tlpProbeCount;
//     SequenceNumber32 m_lastTlpProbeSeq;
// };

// TcpTlpProbeTest::TcpTlpProbeTest()
//     : TestCase("Test TLP probe transmission"),
//       m_tlpProbeCount(0),
//       m_lastTlpProbeSeq(0)
// {
// }

// void
// TcpTlpProbeTest::TlpProbeSent(SequenceNumber32 seq, uint32_t probeCount)
// {
//     NS_LOG_INFO("TLP probe sent: seq=" << seq << " count=" << probeCount);
//     m_tlpProbeCount++;
//     m_lastTlpProbeSeq = seq;
// }

// void
// TcpTlpProbeTest::DoRun()
// {
//     // Create nodes
//     Ptr<Node> node1 = CreateObject<Node>();
//     Ptr<Node> node2 = CreateObject<Node>();
    
//     // Install internet stack
//     InternetStackHelper internet;
//     internet.Install(node1);
//     internet.Install(node2);
    
//     // Create simple network devices
//     Ptr<SimpleNetDevice> dev1 = CreateObject<SimpleNetDevice>();
//     Ptr<SimpleNetDevice> dev2 = CreateObject<SimpleNetDevice>();
//     Ptr<SimpleChannel> channel = CreateObject<SimpleChannel>();
    
//     dev1->SetChannel(channel);
//     dev2->SetChannel(channel);
//     dev1->SetNode(node1);
//     dev2->SetNode(node2);
    
//     node1->AddDevice(dev1);
//     node2->AddDevice(dev2);
    
//     // Assign IP addresses
//     Ipv4AddressHelper ipv4;
//     ipv4.SetBase("10.1.1.0", "255.255.255.0");
    
//     NetDeviceContainer devices;
//     devices.Add(dev1);
//     devices.Add(dev2);
    
//     Ipv4InterfaceContainer interfaces = ipv4.Assign(devices);
    
//     // Create TCP socket on node1 - FIXED: Use proper casting
//     Ptr<Socket> socket = Socket::CreateSocket(node1, TcpSocketFactory::GetTypeId());
//     Ptr<TcpSocketBase> tcpSocket = DynamicCast<TcpSocketBase>(socket);
    
//     tcpSocket->SetAttribute("TlpEnabled", BooleanValue(true));
//     tcpSocket->SetAttribute("TlpTimeoutMin", TimeValue(MilliSeconds(10)));
    
//     // Connect to trace sources
//     tcpSocket->TraceConnectWithoutContext("TlpTx", MakeCallback(&TcpTlpProbeTest::TlpProbeSent, this));
    
//     // Bind and connect
//     Ipv4Address remoteAddr = interfaces.GetAddress(1);
//     socket->Bind();
//     socket->Connect(InetSocketAddress(remoteAddr, 9));
    
//     // Send some data to create outstanding packets - FIXED: Use proper Send signature
//     Ptr<Packet> pkt1 = Create<Packet>(1000);
//     socket->Send(pkt1, 0);  // FIXED: Add flags parameter
    
//     Ptr<Packet> pkt2 = Create<Packet>(1000);
//     socket->Send(pkt2, 0);  // FIXED: Add flags parameter
    
//     // Advance time to trigger TLP
//     Simulator::Stop(Seconds(0.1)); // Should trigger TLP timeout
//     Simulator::Run();
    
//     // Verify TLP probe was sent
//     NS_TEST_ASSERT_MSG_GT(m_tlpProbeCount, 0, "TLP probe should have been sent");
//     NS_LOG_INFO("TLP probes sent: " << m_tlpProbeCount);
    
//     Simulator::Destroy();
// }

// // ===========================================================================
// class TcpTlpProbeTestSuite : public TestSuite
// {
// public:
//     TcpTlpProbeTestSuite()
//         : TestSuite("tcp-tlp-probe", Type::UNIT)
//     {
//         AddTestCase(new TcpTlpProbeTest, TestCase::Duration::EXTENSIVE);
//     }
// };

// static TcpTlpProbeTestSuite g_tcpTlpProbeTestSuite;