#include "m3u_exporter.h"
#include <QFile>
#include <QTextStream>
#include <QJsonArray>
#include <QJsonObject>
#include <QDir>
#include <QDebug>
#include <QStringList>

namespace Iptv::Export {

bool M3uExporter::exportToFile(const QString &path, const QJsonArray &channelList,
                               const ExportOptions &opts)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << path;
        return false;
    }
    
    QTextStream stream(&file);
    //写入M3U文件头
    stream << "#EXTM3U\n";
    
    QStringList processedNames;
    
    for (const QJsonValue &val : channelList) {
        QJsonObject ch = val.toObject();
        
        QString name = ch.value("name").toString();
        QString ip = ch.value("ip").toString();
        int port = ch.value("port").toInt();
        int channelId = ch.value("channel_id").toInt();
        QString group = ch.value("group").toString();
        QString logoName = ch.value("logo_name").toString();
        int type = ch.value("type").toInt();
        
        //合并模式下跳过重复频道
        if (opts.mergeChannels) {
            if (processedNames.contains(name)) {
                continue;
            }
            processedNames.append(name);
        }
        
        //构建显示名称（可选添加高清后缀）
        QString displayName = name;
        if (opts.addHdSuffix) {
            QString typeName;
            switch (type) {
            case 2: typeName = " HD"; break;
            case 3: typeName = " 4K"; break;
            case 4: typeName = " 8K"; break;
            }
            displayName += typeName;
        }
        
        //构建流媒体URL
        QString url = QString("%1/%2/%3:%4")
            .arg(opts.urlPrefix, opts.urlType, ip)
            .arg(port);
        
        if (opts.fccEnabled && !opts.fccUrl.isEmpty()) {
            url += "?fcc=" + opts.fccUrl;
        }
        
        //写入EXTINF标签
        stream << "#EXTINF:-1 tvg-id=\"" << channelId << "\" tvg-name=\"" << name << "\"";
        
        //可选添加台标URL
        if (opts.addLogo && !logoName.isEmpty() && !opts.logoBaseUrl.isEmpty()) {
            stream << " tvg-logo=\"" << opts.logoBaseUrl << "/" << logoName << "\"";
        }
        
        stream << " group-title=\"" << group << "\"," << displayName << "\n";
        stream << url << "\n";
    }
    
    file.close();
    return true;
}

bool M3uExporter::exportToDirectory(const QString &dirPath, const QJsonArray &channelList,
                                    const ExportOptions &opts)
{
    QDir dir(dirPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    //按channel_id分组
    QMap<int, QJsonArray> channelGroups;
    for (const QJsonValue &val : channelList) {
        QJsonObject ch = val.toObject();
        int channelId = ch.value("channel_id").toInt();
        channelGroups[channelId].append(ch);
    }
    
    //每个频道导出为独立M3U文件
    for (auto it = channelGroups.begin(); it != channelGroups.end(); ++it) {
        const QJsonArray &channels = it.value();
        if (channels.isEmpty()) continue;
        
        QJsonObject first = channels.first().toObject();
        int channelId = first.value("channel_id").toInt();
        QString name = first.value("name").toString();
        int type = first.value("type").toInt();
        
        //根据类型生成文件名后缀
        QString typeName;
        switch (type) {
        case 0: typeName = "TS"; break;
        case 1: typeName = "SD"; break;
        case 2: typeName = "HD"; break;
        case 3: typeName = "4K"; break;
        case 4: typeName = "8K"; break;
        default: typeName = "Unknown"; break;
        }
        
        QString fileName = QString("%1_%2_%3.m3u").arg(channelId).arg(name).arg(typeName);
        QString filePath = dirPath + "/" + fileName;
        
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "Failed to open file:" << filePath;
            continue;
        }
        
        QTextStream stream(&file);
        stream << "#EXTM3U\n";
        
        for (const QJsonValue &val : channels) {
            QJsonObject ch = val.toObject();
            QString chName = ch.value("name").toString();
            QString ip = ch.value("ip").toString();
            int port = ch.value("port").toInt();
            int chId = ch.value("channel_id").toInt();
            QString logoName = ch.value("logo_name").toString();
            
            QString url = QString("%1/%2/%3:%4")
                .arg(opts.urlPrefix, opts.urlType, ip)
                .arg(port);
            
            if (opts.fccEnabled && !opts.fccUrl.isEmpty()) {
                url += "?fcc=" + opts.fccUrl;
            }
            
            stream << "#EXTINF:-1 tvg-id=\"" << chId << "\" tvg-name=\"" << chName << "\"";
            
            if (opts.addLogo && !logoName.isEmpty() && !opts.logoBaseUrl.isEmpty()) {
                stream << " tvg-logo=\"" << opts.logoBaseUrl << "/" << logoName << "\"";
            }
            
            stream << " group-title=\"" << first.value("group").toString() << "\"," << chName << "\n";
            stream << url << "\n";
        }
        
        file.close();
    }
    
    return true;
}

QString M3uExporter::fileFilter() const
{
    return "m3u(*.m3u)";
}

} // namespace Iptv::Export
