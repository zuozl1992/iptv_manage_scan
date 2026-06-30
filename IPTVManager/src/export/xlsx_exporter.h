#pragma once

#include <QAbstractItemModel>
#include <QObject>
#include <QString>

namespace Iptv::Export {

class XlsxExporter : public QObject
{
    Q_OBJECT

public:
    explicit XlsxExporter(QObject *parent = nullptr);

    bool exportToFile(const QString &path,
                      QAbstractItemModel *queryModel,
                      QAbstractItemModel *tableModel);
    QString fileFilter() const;

signals:
    void progressChanged(int percent);

private:
    bool writeSheet(const QString &sheetName, QAbstractItemModel *model, void *sheet);
};

} // namespace Iptv::Export
