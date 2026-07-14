#include "xlsx_exporter.h"
#include <QAbstractItemModel>
#include <QDebug>

#include "xlsxdocument.h"
#include "xlsxformat.h"

namespace Iptv::Export {

XlsxExporter::XlsxExporter(QObject *parent)
    : QObject(parent)
{
}

bool XlsxExporter::exportToFile(const QString &path,
                                 QAbstractItemModel *queryModel,
                                 QAbstractItemModel *tableModel)
{
    Q_UNUSED(tableModel)
    
    if (!queryModel) {
        qWarning() << "XLSX export failed: no model provided";
        return false;
    }

    QXlsx::Document xlsx;
    QXlsx::Format headerFormat;
    headerFormat.setFontBold(true);
    headerFormat.setPatternBackgroundColor(QColor(227, 242, 253));

    //写入表头
    for (int col = 0; col < queryModel->columnCount(); ++col) {
        QString header = queryModel->headerData(col, Qt::Horizontal).toString();
        xlsx.write(1, col + 1, header, headerFormat);
    }

    //写入数据
    for (int row = 0; row < queryModel->rowCount(); ++row) {
        for (int col = 0; col < queryModel->columnCount(); ++col) {
            QModelIndex idx = queryModel->index(row, col);
            QVariant value = queryModel->data(idx);
            xlsx.write(row + 2, col + 1, value);
        }
    }

    //保存文件
    bool success = xlsx.saveAs(path);
    if (success) {
        qInfo() << "XLSX exported to:" << path;
    } else {
        qWarning() << "XLSX export failed to save:" << path;
    }
    return success;
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
    
    //TODO: 实现多sheet导出
    return false;
}

} // namespace Iptv::Export
