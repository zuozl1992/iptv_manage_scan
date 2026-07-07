#include "txt_exporter.h"
#include <QFile>
#include <QTextStream>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QStringList>

namespace Iptv::Export {

bool TxtExporter::exportToFile(const QString &path,
                               const QHash<QString, QJsonArray> &groupedLists,
                               const ExportOptions &opts)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << path;
        return false;
    }
    
    QTextStream stream(&file);
    QStringList processedNames;
    
    for (auto it = groupedLists.begin(); it != groupedLists.end(); ++it) {
        const QString &groupName = it.key();
        const QJsonArray &channels = it.value();
        
        if (channels.isEmpty()) continue;
        
        //写入分组标题行
        stream << groupName << ",#genre#\n";
        
        //写入频道数据
        for (const QJsonValue &val : channels) {
            QJsonObject ch = val.toObject();
            QString name = ch.value("name").toString();
            QString ip = ch.value("ip").toString();
            int port = ch.value("port").toInt();
            
            //合并模式下跳过重复频道
            if (opts.mergeChannels) {
                if (processedNames.contains(name)) {
                    continue;
                }
                processedNames.append(name);
            }
            
            //构建流媒体URL
            QString url = QString("%1/%2/%3:%4")
                .arg(opts.urlPrefix, opts.urlType, ip)
                .arg(port);
            
            if (opts.fccEnabled && !opts.fccUrl.isEmpty()) {
                url += "?fcc=" + opts.fccUrl;
            }
            
            stream << name << "," << url << "\n";
        }
        
        //分组间空行分隔
        stream << "\n";
    }
    
    file.close();
    return true;
}

bool TxtExporter::exportToFile(const QString &path,
                               const QHash<QString, QJsonArray> &groupedLists,
                               const ExportOptions &opts,
                               const QStringList &groupOrder)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << path;
        return false;
    }
    
    QTextStream stream(&file);
    QStringList processedNames;
    
    //按指定分组顺序遍历
    for (const QString &groupName : groupOrder) {
        if (!groupedLists.contains(groupName)) continue;
        
        const QJsonArray &channels = groupedLists[groupName];
        if (channels.isEmpty()) continue;
        
        //写入分组标题行
        stream << groupName << ",#genre#\n";
        
        //写入频道数据
        for (const QJsonValue &val : channels) {
            QJsonObject ch = val.toObject();
            QString name = ch.value("name").toString();
            QString ip = ch.value("ip").toString();
            int port = ch.value("port").toInt();
            
            //合并模式下跳过重复频道
            if (opts.mergeChannels) {
                if (processedNames.contains(name)) {
                    continue;
                }
                processedNames.append(name);
            }
            
            //构建流媒体URL
            QString url = QString("%1/%2/%3:%4")
                .arg(opts.urlPrefix, opts.urlType, ip)
                .arg(port);
            
            if (opts.fccEnabled && !opts.fccUrl.isEmpty()) {
                url += "?fcc=" + opts.fccUrl;
            }
            
            stream << name << "," << url << "\n";
        }
        
        //分组间空行分隔
        stream << "\n";
    }
    
    file.close();
    return true;
}

QString TxtExporter::fileFilter() const
{
    return "txt(*.txt)";
}

} // namespace Iptv::Export
