#pragma once

#include "exporter.h"
#include <QHash>
#include <QJsonArray>
#include <QString>
#include <QStringList>

namespace Iptv::Export {

class TxtExporter
{
public:
    bool exportToFile(const QString &path,
                      const QHash<QString, QJsonArray> &groupedLists,
                      const ExportOptions &opts);
    bool exportToFile(const QString &path,
                      const QHash<QString, QJsonArray> &groupedLists,
                      const ExportOptions &opts,
                      const QStringList &groupOrder);
    QString fileFilter() const;
};

} // namespace Iptv::Export
