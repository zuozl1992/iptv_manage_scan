#pragma once

#include "exporter.h"
#include <QJsonArray>
#include <QString>
#include <QDir>

namespace Iptv::Export {

class M3uExporter
{
public:
    bool exportToFile(const QString &path, const QJsonArray &channelList,
                      const ExportOptions &opts);
    bool exportToDirectory(const QString &dirPath, const QJsonArray &channelList,
                           const ExportOptions &opts);
    QString fileFilter() const;
};

} // namespace Iptv::Export
