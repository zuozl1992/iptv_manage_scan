#include <QtTest>
#include "core/config/appconfig.h"
#include "core/config/bootstrap.h"

class TestConfig : public QObject
{
    Q_OBJECT

private slots:
    void testBootstrap()
    {
        Iptv::Core::Bootstrap bootstrap;
        bootstrap.setConfigPath("/tmp/test_config.ini");
        bootstrap.setDbPath("/tmp/test.db");
        
        QCOMPARE(bootstrap.configPath(), QString("/tmp/test_config.ini"));
        QCOMPARE(bootstrap.dbPath(), QString("/tmp/test.db"));
    }

    void testAppConfig()
    {
        Iptv::Core::AppConfig *config = Iptv::Core::AppConfig::instance();
        // TODO: Test with actual config file
        QVERIFY(config != nullptr);
    }
};

QTEST_MAIN(TestConfig)
#include "test_config.moc"
