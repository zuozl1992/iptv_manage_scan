#include "url_builder.h"

#include <QFile>
#include <QRegularExpression>

namespace Iptv::Logic {

QStringList UrlBuilder::expand(const QString &templ)
{
    QString temp = templ;
    if (temp.split("{").length() > 4)
        return {};

    int sIndex = temp.indexOf("{");
    int eIndex = temp.indexOf("}");
    if (sIndex < 0 && eIndex < 0) {
        // No braces - treat the whole string as a single pattern
        // This handles cases like "1#3#[5-7]" directly
        QStringList expanded = expandRange(temp);
        return expanded;
    } else if (sIndex < 0 || eIndex < 0 || eIndex <= sIndex) {
        return {};
    }

    QStringList strList = expandRange(temp.mid(sIndex + 1, eIndex - sIndex - 1));
    QString a = temp.mid(0, sIndex);
    QString b = temp.mid(eIndex + 1);

    QStringList result;
    if (b.indexOf("{") >= 0) {
        QStringList tre = expand(b);
        if (tre.isEmpty())
            return {};
        for (int i = 0; i < strList.length(); i++) {
            for (int j = 0; j < tre.length(); j++) {
                result.append(QString("%1%2%3")
                    .arg(a)
                    .arg(strList.at(i))
                    .arg(tre.at(j)));
            }
        }
    } else {
        for (int i = 0; i < strList.length(); i++) {
            result.append(QString("%1%2%3")
                .arg(a)
                .arg(strList.at(i))
                .arg(b));
        }
    }
    return result;
}

QStringList UrlBuilder::expandFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return {};

    QStringList result;
    while (true) {
        QByteArray data = file.readLine();
        if (data.isEmpty())
            break;
        if (data.lastIndexOf('\n') >= 0)
            data.removeLast();
        if (data.lastIndexOf('\r') >= 0)
            data.removeLast();
        QStringList expanded = expand(QString::fromLocal8Bit(data));
        if (expanded.isEmpty())
            return {};
        result.append(expanded);
    }
    return result;
}

QString UrlBuilder::getIpFromUrl(const QString &url)
{
    // Handle udp:// and rtp:// schemes - extract IP after the scheme
    // "udp://239.49.0.1:6000" -> "239.49.0.1"
    // "rtp://239.49.0.1:6000" -> "239.49.0.1"
    // "http://192.168.1.1:12345/udp/239.49.0.1:6000" -> "239.49.0.1"
    
    // First check for udp:// or rtp:// at the beginning
    if (url.startsWith("udp://") || url.startsWith("rtp://")) {
        int start = 6; // Skip "udp://" or "rtp://"
        int end = url.indexOf(":", start);
        if (end > start) {
            return url.mid(start, end - start);
        }
    }
    
    // For URLs like "http://.../udp/..." or "http://.../rtp/..."
    int i1 = url.indexOf("/udp/");
    int i2 = url.indexOf("/rtp/");
    if (i1 >= 0 || i2 >= 0) {
        int start = (i1 < 0 ? i2 : i1) + 5; // Skip "/udp/" or "/rtp/"
        int end = url.indexOf(":", start);
        if (end > start) {
            return url.mid(start, end - start);
        }
    }
    
    // Fallback: try to find IP pattern after "://"
    int schemeEnd = url.indexOf("://");
    if (schemeEnd >= 0) {
        int start = schemeEnd + 3;
        int end = url.indexOf(":", start);
        if (end > start) {
            return url.mid(start, end - start);
        }
    }
    
    return url;
}

QStringList UrlBuilder::expandRange(const QString &pattern)
{
    QStringList result;

    // Check for # separated values (may contain ranges)
    if (pattern.contains("#")) {
        QStringList parts = pattern.split("#");
        for (const QString &part : parts) {
            if (part.startsWith("[")) {
                // Range pattern within mixed notation
                QString r = part.mid(1, part.length() - 2);
                QStringList rl = r.remove(QRegularExpression("\\s")).split("-");
                if (rl.length() != 2)
                    continue;
                int s = rl.at(0).toInt();
                int e = rl.at(1).toInt();
                int len = rl.at(0).length();
                if (s >= e || s <= 0 || e <= 0)
                    continue;
                for (int j = s; j <= e; j++)
                    result.append(QString("%1").arg(j, len, 10, QChar('0')));
            } else {
                result.append(part);
            }
        }
        return result;
    }

    // Check for range pattern: [start-end]
    if (pattern.startsWith("[")) {
        QString r = pattern.mid(1, pattern.length() - 2);
        QStringList rl = r.remove(QRegularExpression("\\s")).split("-");
        if (rl.length() != 2)
            return {pattern};
        int s = rl.at(0).toInt();
        int e = rl.at(1).toInt();
        int len = rl.at(0).length();
        if (s >= e || s <= 0 || e <= 0)
            return {pattern};
        for (int j = s; j <= e; j++)
            result.append(QString("%1").arg(j, len, 10, QChar('0')));
        return result;
    }

    // Single value
    result << pattern;
    return result;
}

} // namespace Iptv::Logic
