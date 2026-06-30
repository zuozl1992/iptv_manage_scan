#include <QtTest>
#include "logic/url_builder.h"

class TestUrlBuilder : public QObject
{
    Q_OBJECT

private slots:
    void testExpandRange()
    {
        QStringList result = Iptv::Logic::UrlBuilder::expand("192.168.1.{1-3}");
        QCOMPARE(result.size(), 3);
        QCOMPARE(result[0], "192.168.1.1");
        QCOMPARE(result[1], "192.168.1.2");
        QCOMPARE(result[2], "192.168.1.3");
    }

    void testExpandComma()
    {
        QStringList result = Iptv::Logic::UrlBuilder::expand("192.168.{1#2#3}.1");
        QCOMPARE(result.size(), 3);
        QCOMPARE(result[0], "192.168.1.1");
        QCOMPARE(result[1], "192.168.2.1");
        QCOMPARE(result[2], "192.168.3.1");
    }

    void testExpandNoPattern()
    {
        QStringList result = Iptv::Logic::UrlBuilder::expand("192.168.1.1");
        QCOMPARE(result.size(), 1);
        QCOMPARE(result[0], "192.168.1.1");
    }
};

QTEST_MAIN(TestUrlBuilder)
#include "test_url_builder.moc"
