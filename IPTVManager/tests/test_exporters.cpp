#include <QtTest>
#include "export/csv_exporter.h"

class TestExporters : public QObject
{
    Q_OBJECT

private slots:
    void testCsvExporter()
    {
        // TODO: Implement test
        QVERIFY(true);
    }

    void testM3uExporter()
    {
        // TODO: Implement test
        QVERIFY(true);
    }
};

QTEST_MAIN(TestExporters)
#include "test_exporters.moc"
