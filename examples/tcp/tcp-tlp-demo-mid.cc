#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/error-model.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TcpTlpDemo");

class TailLossErrorModel : public ErrorModel
{
public:
    static TypeId GetTypeId();
    TailLossErrorModel() : m_dataCount(0) {}
    
private:
    bool DoCorrupt(Ptr<Packet> p) override;
    void DoReset() override {}
    
    uint32_t m_dataCount;
};

TypeId
TailLossErrorModel::GetTypeId()
{
    static TypeId tid = TypeId("TailLossErrorModel")
        .SetParent<ErrorModel>()
        .SetGroupName("Network")
        .AddConstructor<TailLossErrorModel>();
    return tid;
}

bool
TailLossErrorModel::DoCorrupt(Ptr<Packet> p)
{
    // Only consider large data packets (590 bytes)
    if (p->GetSize() >= 500) {
        m_dataCount++;
        std::cout << "DATA PACKET " << m_dataCount << " at " << Simulator::Now().GetSeconds() 
                  << "s, size: " << p->GetSize() << " bytes" << std::endl;
        
        // Drop packet to create tail loss (not the very last one)
        if (m_dataCount >= 6 && m_dataCount <=7) {
            std::cout << "*** DROPPING DATA PACKET "<<m_dataCount<<" - CREATING TAIL LOSS ***" << std::endl;
            return true;
        }
    }
    
    return false;
}

int main(int argc, char *argv[])
{
    bool enableTlp = true;
    std::string traceFile = "tcp-tlp-demo.tr";
    
    CommandLine cmd(__FILE__);
    cmd.AddValue("enable-tlp", "Enable Tail Loss Probe", enableTlp);
    cmd.AddValue("trace-file", "ASCII trace file name", traceFile);
    cmd.Parse(argc, argv);
    
    std::cout << "=== TCP TLP - REAL TAIL LOSS ===" << std::endl;
    std::cout << "TLP: " << (enableTlp ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "Dropping DATA PACKET 4 (590-byte packet)" << std::endl;
    std::cout << "This should create tail loss that TLP can recover from" << std::endl;
    std::cout << std::endl;
    
    NodeContainer nodes;
    nodes.Create(2);
    
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("100ms")); // High delay
    
    NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes);
    
    // Error model on device 1 (where data packets flow: node0->node1)
    Ptr<TailLossErrorModel> em = CreateObject<TailLossErrorModel>();
    devices.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(em));
    
    InternetStackHelper stack;
    stack.Install(nodes);
    
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.252");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);
    
    // Setup applications with longer timeout
    uint16_t sinkPort = 8080;
    Address sinkAddress(InetSocketAddress(interfaces.GetAddress(1), sinkPort));
    
    PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), sinkPort));
    ApplicationContainer sinkApps = sinkHelper.Install(nodes.Get(1));
    sinkApps.Start(Seconds(1.0));
    sinkApps.Stop(Seconds(30.0));
    
    BulkSendHelper sourceHelper("ns3::TcpSocketFactory", sinkAddress);
    sourceHelper.SetAttribute("MaxBytes", UintegerValue(4000));
    ApplicationContainer sourceApps = sourceHelper.Install(nodes.Get(0));
    
    sourceApps.Start(Seconds(2.0));
    sourceApps.Stop(Seconds(25.0));
    
    // Configure TLP
    Config::SetDefault("ns3::TcpSocketBase::TlpEnabled", BooleanValue(enableTlp));
    Config::SetDefault("ns3::TcpSocketBase::TlpTimeoutMin", TimeValue(MilliSeconds(10)));
    
    // Enable tracing
    AsciiTraceHelper ascii;
    pointToPoint.EnableAsciiAll(ascii.CreateFileStream(traceFile));
    
    Simulator::Stop(Seconds(30.0));
    Simulator::Run();
    
    Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkApps.Get(0));
    
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Bytes received: " << sink->GetTotalRx() << "/4000" << std::endl;
    
    if (sink->GetTotalRx() >= 4000) {
        std::cout << "TRANSFER COMPLETED" << std::endl;
    } else {
        std::cout << "TRANSFER INCOMPLETE - tail loss prevented completion!" << std::endl;
    }
    
    std::cout << "Check " << traceFile << " for TLP vs RTO behavior" << std::endl;
    
    Simulator::Destroy();
    
    return 0;
}