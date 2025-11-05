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
 * \brief TCP TLP Loss Recovery Test
 */
class TcpTlpLossRecoveryTest : public TestCase
{
public:
    TcpTlpLossRecoveryTest();

private:
    virtual void DoRun(void);
};

TcpTlpLossRecoveryTest::TcpTlpLossRecoveryTest()
    : TestCase("TcpTlpLossRecoveryTest")
{
}

void
TcpTlpLossRecoveryTest::DoRun()
{
    // Test that TLP state variables are properly initialized
    Ptr<TcpSocketBase> socket = CreateObject<TcpSocketBase>();
    socket->SetAttribute("TlpEnabled", BooleanValue(true));
    
    // After enabling TLP, verify initial state
    // (You might need to add getter methods to TcpSocketBase for these)
    // For now, we'll test that the socket can be created with TLP enabled
    
    BooleanValue tlpEnabled;
    socket->GetAttribute("TlpEnabled", tlpEnabled);
    NS_TEST_ASSERT_MSG_EQ(tlpEnabled.Get(), true, "TLP should be enabled for loss recovery test");
    
    // Test that we can configure TLP parameters
    socket->SetAttribute("TlpMaxProbes", UintegerValue(2));
    UintegerValue maxProbes;
    socket->GetAttribute("TlpMaxProbes", maxProbes);
    NS_TEST_ASSERT_MSG_EQ(maxProbes.Get(), 2, "Should be able to set max probes");
    
    Simulator::Destroy();
}

// ===========================================================================
class TcpTlpLossRecoveryTestSuite : public TestSuite
{
public:
    TcpTlpLossRecoveryTestSuite()
        : TestSuite("tcp-tlp-loss-recovery", Type::UNIT)
    {
        AddTestCase(new TcpTlpLossRecoveryTest, TestCase::Duration::QUICK);
    }
};

static TcpTlpLossRecoveryTestSuite g_tcpTlpLossRecoveryTestSuite;