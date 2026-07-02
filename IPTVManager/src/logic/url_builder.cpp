#include "url_builder.h"
#include <QRegularExpression>
#include <QDebug>

namespace Iptv::Logic {

QStringList UrlBuilder::expand(const QString &templ)
{
    QStringList result;
    result << templ;
    
    // Find {...} patterns and expand them
    QRegularExpression re("\\{([^}]+)\\}");
    
    while (true) {
        bool expanded = false;
        QStringList newResult;
        
        for (const QString &str : result) {
            QRegularExpressionMatch match = re.match(str);
            if (match.hasMatch()) {
                expanded = true;
                QString pattern = match.captured(1);
                QStringList expandedValues = expandRange(pattern);
                
                for (const QString &value : expandedValues) {
                    QString newStr = str;
                    newStr.replace(match.capturedStart(), match.capturedLength(), value);
                    newResult << newStr;
                }
            } else {
                newResult << str;
            }
        }
        
        result = newResult;
        
        if (!expanded) break;
    }
    
    return result;
}

QStringList UrlBuilder::buildScanIps(const QString &groupAddress,
                                     const QString &port,
                                     const QStringList &existingIps)
{
    QStringList prefixes = expand(groupAddress);
    QStringList result;
    
    for (const QString &prefix : prefixes) {
        for (int j = 1; j <= 255; j++) {
            QString ip = prefix + QString(".%1").arg(j);
            if (existingIps.contains(ip))
                continue;
            if (port.isEmpty())
                result << ip;
            else
                result << QString("%1:{%2}").arg(ip).arg(port);
        }
    }
    
    return result;
}

QStringList UrlBuilder::buildTestUrls(const QString &port,
                                      const QStringList &existingIps)
{
    Q_UNUSED(port)
    Q_UNUSED(existingIps)
    // This would need to query the database for test URLs
    // Implementation depends on database access
    return {};
}

QStringList UrlBuilder::expandRange(const QString &pattern)
{
    QStringList result;
    
    // Check for comma-separated values
    if (pattern.contains("#")) {
        return pattern.split("#");
    }
    
    // Check for range pattern: start-end
    QRegularExpression rangeRe("^(\\d+)-(\\d+)$");
    QRegularExpressionMatch rangeMatch = rangeRe.match(pattern);
    
    if (rangeMatch.hasMatch()) {
        int start = rangeMatch.captured(1).toInt();
        int end = rangeMatch.captured(2).toInt();
        
        // Check for zero-padding
        int padLength = 0;
        if (pattern.startsWith("0") || pattern.contains("-0")) {
            padLength = qMax(rangeMatch.captured(1).length(),
                            rangeMatch.captured(2).length());
        }
        
        for (int i = start; i <= end; ++i) {
            if (padLength > 0) {
                result << QString("%1").arg(i, padLength, 10, QChar('0'));
            } else {
                result << QString::number(i);
            }
        }
        
        return result;
    }
    
    // Single value
    result << pattern;
    return result;
}

} // namespace Iptv::Logic
