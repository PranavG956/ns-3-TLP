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
 * \brief TCP TLP Basic Test
 */
class TcpTlpBasicTest : public TestCase
{
public:
    TcpTlpBasicTest();

private:
    virtual void DoRun(void);
};

TcpTlpBasicTest::TcpTlpBasicTest()
    : TestCase("TcpTlpBasicTest")
{
}

void
TcpTlpBasicTest::DoRun()
{
    // Test 1: TLP enabled by attribute
    Ptr<TcpSocketBase> socket = CreateObject<TcpSocketBase>();
    socket->SetAttribute("TlpEnabled", BooleanValue(true));
    
    BooleanValue tlpEnabled;
    socket->GetAttribute("TlpEnabled", tlpEnabled);
    NS_TEST_ASSERT_MSG_EQ(tlpEnabled, true, "TLP should be enabled");
    
    // Test 2: Default TLP is disabled
    Ptr<TcpSocketBase> socket2 = CreateObject<TcpSocketBase>();
    socket2->GetAttribute("TlpEnabled", tlpEnabled);
    NS_TEST_ASSERT_MSG_EQ(tlpEnabled, false, "TLP should be disabled by default");
    
    Simulator::Destroy();
}

// ===========================================================================
// Test Suite for TLP functionality
// ===========================================================================
class TlpTestSuite : public TestSuite
{
public:
    TlpTestSuite()
        : TestSuite("tcp-tlp", UNIT)
    {
        AddTestCase(new TcpTlpBasicTest, TestCase::QUICK);
    }
};

static TlpTestSuite g_tlpTestSuite;