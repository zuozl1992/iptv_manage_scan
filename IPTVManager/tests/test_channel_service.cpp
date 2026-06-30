#include <QtTest>
#include "logic/channel_service.h"

class TestChannelService : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        // Setup test database
    }

    void testImportFromTxt()
    {
        // TODO: Implement test
        QVERIFY(true);
    }

    void cleanupTestCase()
    {
        // Cleanup
    }
};

QTEST_MAIN(TestChannelService)
#include "test_channel_service.moc"
