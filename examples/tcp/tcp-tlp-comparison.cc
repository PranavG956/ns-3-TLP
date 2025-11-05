#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/error-model.h"
#include <iomanip>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TlpAccurateDemo");

class AccurateTailLossErrorModel : public ErrorModel
{
public:
    static TypeId GetTypeId();
    AccurateTailLossErrorModel() : m_originalDataCount(0), m_totalDataCount(0) {}
    
    uint32_t GetOriginalDataCount() const { return m_originalDataCount; }
    uint32_t GetTotalDataCount() const { return m_totalDataCount; }
    
private:
    bool DoCorrupt(Ptr<Packet> p) override;
    void DoReset() override {}
    
    uint32_t m_originalDataCount; // Only first transmission of data packets
    uint32_t m_totalDataCount;    // All data packets (including retransmissions)
};

TypeId
AccurateTailLossErrorModel::GetTypeId()
{
    static TypeId tid = TypeId("AccurateTailLossErrorModel")
        .SetParent<ErrorModel>()
        .SetGroupName("Network")
        .AddConstructor<AccurateTailLossErrorModel>();
    return tid;
}

bool
AccurateTailLossErrorModel::DoCorrupt(Ptr<Packet> p)
{
    if (p->GetSize() >= 500) {
        m_totalDataCount++;
        
        // Try to detect if this is an original transmission or retransmission
        // This is simplified - in real implementation you'd check sequence numbers
        if (m_totalDataCount <= 7) { // First 7 are original transmissions
            m_originalDataCount++;
            std::cout << "ORIGINAL DATA PACKET " << m_originalDataCount << " at " 
                      << Simulator::Now().GetSeconds() << "s, size: " << p->GetSize() << " bytes" << std::endl;
            
            if (m_originalDataCount == 7) {
                std::cout << "*** DROPPING ORIGINAL PACKET 7 ***" << std::endl;
                return true;
            }
        } else {
            std::cout << "RETRANSMISSION/RECOVERY PACKET at " << Simulator::Now().GetSeconds() 
                      << "s, size: " << p->GetSize() << " bytes" << std::endl;
        }
    }
    
    return false;
}

int main(int argc, char *argv[])
{
    bool enableTlp = true;
    
    CommandLine cmd(__FILE__);
    cmd.AddValue("enable-tlp", "Enable Tail Loss Probe", enableTlp);
    cmd.Parse(argc, argv);
    
    std::cout << "=== ACCURATE TLP DEMONSTRATION ===" << std::endl;
    std::cout << "TLP: " << (enableTlp ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "Data: 4000 bytes, Segment size: ~590 bytes" << std::endl;
    std::cout << "Expected: 7 packets (4130 bytes) but we limit to 4000 bytes" << std::endl;
    std::cout << std::endl;
    
    NodeContainer nodes;
    nodes.Create(2);
    
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("100ms"));
    
    NetDeviceContainer devices = pointToPoint.Install(nodes);
    
    Ptr<AccurateTailLossErrorModel> em = CreateObject<AccurateTailLossErrorModel>();
    devices.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(em));
    
    InternetStackHelper stack;
    stack.Install(nodes);
    
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.252");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);
    
    uint16_t sinkPort = 8080;
    Address sinkAddress(InetSocketAddress(interfaces.GetAddress(1), sinkPort));
    
    PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), sinkPort));
    ApplicationContainer sinkApps = sinkHelper.Install(nodes.Get(1));
    sinkApps.Start(Seconds(1.0));
    sinkApps.Stop(Seconds(30.0));
    
    BulkSendHelper sourceHelper("ns3::TcpSocketFactory", sinkAddress);
    sourceHelper.SetAttribute("MaxBytes", UintegerValue(4000)); // Exactly 4000 bytes
    sourceHelper.SetAttribute("SendSize", UintegerValue(590)); // But segments are 590 bytes
    
    ApplicationContainer sourceApps = sourceHelper.Install(nodes.Get(0));
    sourceApps.Start(Seconds(2.0));
    sourceApps.Stop(Seconds(25.0));
    
    Config::SetDefault("ns3::TcpSocketBase::TlpEnabled", BooleanValue(enableTlp));
    
    Simulator::Stop(Seconds(30.0));
    Simulator::Run();
    
    Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkApps.Get(0));
    
    std::cout << std::endl;
    std::cout << "=== ACCURATE RESULTS ===" << std::endl;
    std::cout << "Original data packets sent: " << em->GetOriginalDataCount() << std::endl;
    std::cout << "Total data packets (including retransmissions): " << em->GetTotalDataCount() << std::endl;
    std::cout << "Bytes received: " << sink->GetTotalRx() << "/4000" << std::endl;
    std::cout << "Transfer: " << (sink->GetTotalRx() >= 4000 ? "COMPLETED" : "FAILED") << std::endl;
    
    Simulator::Destroy();
    
    return 0;
}