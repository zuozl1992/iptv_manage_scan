#include "xlsx_exporter.h"
#include <QAbstractItemModel>
#include <QDebug>

// QXlsx includes - uncomment when QXlsx is installed
// #include "xlsxdocument.h"
// #include "xlsxformat.h"

namespace Iptv::Export {

XlsxExporter::XlsxExporter(QObject *parent)
    : QObject(parent)
{
}

bool XlsxExporter::exportToFile(const QString &path,
                                 QAbstractItemModel *queryModel,
                                 QAbstractItemModel *tableModel)
{
    Q_UNUSED(path)
    Q_UNUSED(queryModel)
    Q_UNUSED(tableModel)
    
    // TODO: Implement with QXlsx library
    // Download from https://github.com/QtExcel/QXlsx
    qWarning() << "XLSX export not available - QXlsx library not installed";
    return false;
}

QString XlsxExporter::fileFilter() const
{
    return "Excel(*.xlsx)";
}

bool XlsxExporter::writeSheet(const QString &sheetName, QAbstractItemModel *model, void *docPtr)
{
    Q_UNUSED(sheetName)
    Q_UNUSED(model)
    Q_UNUSED(docPtr)
    
    // TODO: Implement with QXlsx library
    return false;
}

} // namespace Iptv::Export
