#pragma once

#include <QString>
#include <QStringList>

namespace Iptv::Logic {

class UrlBuilder
{
public:
    static QStringList expand(const QString &templ);
    static QStringList buildScanIps(const QString &groupAddress,
                                    const QString &port,
                                    const QStringList &existingIps);
    static QStringList buildTestUrls(const QString &port,
                                     const QStringList &existingIps);

private:
    static QStringList expandRange(const QString &pattern);
};

} // namespace Iptv::Logic
