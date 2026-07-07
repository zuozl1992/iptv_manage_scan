#include "url_builder.h"
#include <QRegularExpression>
#include <QDebug>

namespace Iptv::Logic {

QStringList UrlBuilder::expand(const QString &templ)
{
    QStringList result;
    result << templ;
    
    //匹配花括号表达式{...}
    QRegularExpression re("\\{([^}]+)\\}");
    
    //循环展开所有嵌套的花括号表达式
    while (true) {
        bool expanded = false;
        QStringList newResult;
        
        for (const QString &str : result) {
            QRegularExpressionMatch match = re.match(str);
            if (match.hasMatch()) {
                expanded = true;
                QString pattern = match.captured(1);
                QStringList expandedValues = expandRange(pattern);
                
                //替换匹配位置为展开后的值
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
    //展开组播地址模板获取IP前缀
    QStringList prefixes = expand(groupAddress);
    QStringList result;
    
    //对每个前缀遍历1-255生成完整IP
    for (const QString &prefix : prefixes) {
        for (int j = 1; j <= 255; j++) {
            QString ip = prefix + QString(".%1").arg(j);
            //过滤数据库中已存在的IP
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
    //需要查询数据库获取测试频道URL
    return {};
}

QStringList UrlBuilder::expandRange(const QString &pattern)
{
    QStringList result;
    
    //检查是否为#分隔的多值表达式
    if (pattern.contains("#")) {
        return pattern.split("#");
    }
    
    //检查是否为范围表达式：start-end
    QRegularExpression rangeRe("^(\\d+)-(\\d+)$");
    QRegularExpressionMatch rangeMatch = rangeRe.match(pattern);
    
    if (rangeMatch.hasMatch()) {
        int start = rangeMatch.captured(1).toInt();
        int end = rangeMatch.captured(2).toInt();
        
        //检测是否需要前导零填充
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
    
    //单个值直接返回
    result << pattern;
    return result;
}

} // namespace Iptv::Logic
