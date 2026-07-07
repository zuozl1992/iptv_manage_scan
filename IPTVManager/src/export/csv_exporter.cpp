#include "csv_exporter.h"
#include <QFile>
#include <QTextStream>
#include <QAbstractItemModel>
#include <QDebug>

namespace Iptv::Export {

bool CsvExporter::exportToFile(const QString &path, QAbstractItemModel *model)
{
    if (!model) return false;
    
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << path;
        return false;
    }
    
    QTextStream stream(&file);
    
    //写入表头
    QStringList headers;
    for (int col = 0; col < model->columnCount(); ++col) {
        headers << model->headerData(col, Qt::Horizontal).toString();
    }
    stream << headers.join(",") << "\n";
    
    //写入数据行
    for (int row = 0; row < model->rowCount(); ++row) {
        QStringList values;
        for (int col = 0; col < model->columnCount(); ++col) {
            QString value = model->data(model->index(row, col)).toString();
            //转义逗号和引号
            if (value.contains(",") || value.contains("\"") || value.contains("\n")) {
                value = "\"" + value.replace("\"", "\"\"") + "\"";
            }
            values << value;
        }
        stream << values.join(",") << "\n";
    }
    
    file.close();
    return true;
}

QString CsvExporter::fileFilter() const
{
    return "CSV(*.csv)";
}

} // namespace Iptv::Export
