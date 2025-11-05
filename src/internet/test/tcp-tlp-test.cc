#include "ns3/test.h"
#include "tcp-tlp-probe-test.h"
#include "tcp-tlp-recovery-test.h"

using namespace ns3;

class TcpTlpIntegrationTestSuite : public TestSuite
{
public:
    TcpTlpIntegrationTestSuite()
        : TestSuite("tcp-tlp-integration", SYSTEM)
    {
        // Unit tests
        AddTestCase(new TcpTlpTest, TestCase::QUICK);
        AddTestCase(new TcpTlpProbeTest, TestCase::EXTENSIVE);
        AddTestCase(new TcpTlpRecoveryTest, TestCase::EXTENSIVE);
    }
};

static TcpTlpIntegrationTestSuite g_tcpTlpIntegrationTestSuite;