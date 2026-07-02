#pragma once

#include <QString>
#include <QStringList>

namespace Iptv::Logic {

class UrlBuilder
{
public:
    static QStringList expand(const QString &templ);
    static QStringList expandFromFile(const QString &filePath);
    static QString getIpFromUrl(const QString &url);

private:
    static QStringList expandRange(const QString &pattern);
};

} // namespace Iptv::Logic
