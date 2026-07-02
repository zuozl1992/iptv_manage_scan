#include <QtTest>
#include "logic/url_builder.h"

using namespace Iptv::Logic;

class TestUrlBuilder : public QObject
{
    Q_OBJECT

private slots:
    void testExpandSingleValue();
    void testExpandHashValues();
    void testExpandRange();
    void testExpandZeroPaddedRange();
    void testExpandNested();
    void testExpandMixedNotation();
    void testGetIpFromUrl();
    void testGetIpFromUrlWithScheme();
};

void TestUrlBuilder::testExpandSingleValue()
{
    QStringList result = UrlBuilder::expand("http://192.168.1.1:8080");
    QCOMPARE(result.size(), 1);
    QCOMPARE(result.first(), QString("http://192.168.1.1:8080"));
}

void TestUrlBuilder::testExpandHashValues()
{
    QStringList result = UrlBuilder::expand("239.49.{0#1#8}");
    QCOMPARE(result.size(), 3);
    QVERIFY(result.contains("239.49.0"));
    QVERIFY(result.contains("239.49.1"));
    QVERIFY(result.contains("239.49.8"));
}

void TestUrlBuilder::testExpandRange()
{
    QStringList result = UrlBuilder::expand("{[1-3]}");
    QCOMPARE(result.size(), 3);
    QCOMPARE(result.at(0), QString("1"));
    QCOMPARE(result.at(1), QString("2"));
    QCOMPARE(result.at(2), QString("3"));
}

void TestUrlBuilder::testExpandZeroPaddedRange()
{
    QStringList result = UrlBuilder::expand("{[08-10]}");
    QCOMPARE(result.size(), 3);
    QCOMPARE(result.at(0), QString("08"));
    QCOMPARE(result.at(1), QString("09"));
    QCOMPARE(result.at(2), QString("10"));
}

void TestUrlBuilder::testExpandNested()
{
    QStringList result = UrlBuilder::expand("{1#2}:{3#4}");
    QCOMPARE(result.size(), 4);
    QVERIFY(result.contains("1:3"));
    QVERIFY(result.contains("1:4"));
    QVERIFY(result.contains("2:3"));
    QVERIFY(result.contains("2:4"));
}

void TestUrlBuilder::testExpandMixedNotation()
{
    QStringList result = UrlBuilder::expand("1#3#[5-7]");
    QCOMPARE(result.size(), 5);
    QVERIFY(result.contains("1"));
    QVERIFY(result.contains("3"));
    QVERIFY(result.contains("5"));
    QVERIFY(result.contains("6"));
    QVERIFY(result.contains("7"));
}

void TestUrlBuilder::testGetIpFromUrl()
{
    QCOMPARE(UrlBuilder::getIpFromUrl("udp://239.49.0.1:6000"),
             QString("239.49.0.1"));
}

void TestUrlBuilder::testGetIpFromUrlWithScheme()
{
    QCOMPARE(UrlBuilder::getIpFromUrl("http://192.168.1.1:12345/udp/239.49.0.1:6000"),
             QString("239.49.0.1"));
}

QTEST_MAIN(TestUrlBuilder)
#include "test_url_builder.moc"
