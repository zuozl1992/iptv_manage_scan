#pragma once

#include <QAbstractItemModel>
#include <QString>

namespace Iptv::Export {

class CsvExporter
{
public:
    bool exportToFile(const QString &path, QAbstractItemModel *model);
    QString fileFilter() const;
};

} // namespace Iptv::Export
