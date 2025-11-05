#include "ns3/test.h"
#include "ns3/tcp-socket-base.h"
#include "ns3/node.h"
#include "ns3/simulator.h"
#include "ns3/boolean.h"

using namespace ns3;

/**
 * \ingroup internet-test
 * \ingroup tests
 *
 * \brief TCP TLP Configuration Test
 */
class TcpTlpConfigTest : public TestCase
{
public:
    TcpTlpConfigTest();

private:
    virtual void DoRun(void);
};

TcpTlpConfigTest::TcpTlpConfigTest()
    : TestCase("TcpTlpConfigTest")
{
}

void
TcpTlpConfigTest::DoRun()
{
    // Test TLP configuration through attributes
    Ptr<TcpSocketBase> socket = CreateObject<TcpSocketBase>();
    
    // Test enabling TLP
    socket->SetAttribute("TlpEnabled", BooleanValue(true));
    BooleanValue tlpEnabled;
    socket->GetAttribute("TlpEnabled", tlpEnabled);
    NS_TEST_ASSERT_MSG_EQ(tlpEnabled.Get(), true, "TLP should be enabled");
    
    // Test TLP max probes
    socket->SetAttribute("TlpMaxProbes", UintegerValue(3));
    UintegerValue maxProbes;
    socket->GetAttribute("TlpMaxProbes", maxProbes);
    NS_TEST_ASSERT_MSG_EQ(maxProbes.Get(), 3, "Max probes should be 3");
    
    // Test TLP timeout
    socket->SetAttribute("TlpTimeoutMin", TimeValue(MilliSeconds(20)));
    TimeValue timeout;
    socket->GetAttribute("TlpTimeoutMin", timeout);
    NS_TEST_ASSERT_MSG_EQ(timeout.Get(), MilliSeconds(20), "Timeout should be 20ms");
    
    Simulator::Destroy();
}

// ===========================================================================
class TcpTlpConfigTestSuite : public TestSuite
{
public:
    TcpTlpConfigTestSuite()
        : TestSuite("tcp-tlp-config", Type::UNIT)
    {
        AddTestCase(new TcpTlpConfigTest, TestCase::Duration::QUICK);
    }
};

static TcpTlpConfigTestSuite g_tcpTlpConfigTestSuite;