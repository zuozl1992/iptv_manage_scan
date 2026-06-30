#pragma once

#include "exporter.h"
#include <QString>
#include <QStringList>

namespace Iptv::Export {

class ScanFileGenerator
{
public:
    bool generate(const QString &path, const QStringList &testUrls,
                  const QStringList &scanIps, const ExportOptions &opts);
};

} // namespace Iptv::Export
