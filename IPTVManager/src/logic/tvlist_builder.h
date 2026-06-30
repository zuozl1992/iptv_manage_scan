#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QString>

namespace Iptv::Export {
struct ExportOptions;
}

namespace Iptv::Logic {

class TvListBuilder
{
public:
    static QJsonArray buildForGroup(const QJsonArray &sources,
                                    const Export::ExportOptions &opts);
    static void insertSorted(QJsonArray &arr, const QJsonObject &obj);
};

} // namespace Iptv::Logic
