#include "channel_service.h"
#include "database/channel_repository.h"
#include "database/source_repository.h"
#include "database/types.h"
#include "export/exporter.h"

#include <QFile>
#include <QTextStream>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

namespace Iptv::Logic {

ChannelService::ChannelService(Database::ChannelRepository *channelRepo,
                               Database::SourceRepository *sourceRepo,
                               QObject *parent)
    : QObject(parent)
    , m_channelRepo(channelRepo)
    , m_sourceRepo(sourceRepo)
{
}

int ChannelService::addChannel(const QString &name)
{
    m_channelRepo->insertChannel(name);
    return m_channelRepo->findIdByName(name);
}

int ChannelService::getOrCreateChannel(const QString &name)
{
    int id = m_channelRepo->findIdByName(name);
    if (id < 0) {
        id = addChannel(name);
    }
    return id;
}

bool ChannelService::addSource(int tvId, const Database::TvSource &source)
{
    Database::TvSource src = source;
    src.tvId = tvId;
    return m_sourceRepo->upsertSource(src);
}

bool ChannelService::removeSource(const QString &ip)
{
    return m_sourceRepo->deleteByIp(ip);
}

bool ChannelService::removeAllSources()
{
    return m_sourceRepo->deleteAll();
}

bool ChannelService::removeAllChannels()
{
    return m_channelRepo->deleteAll();
}

int ChannelService::importFromTxt(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file:" << filePath;
        return -1;
    }
    
    int count = 0;
    int errCount = 0;
    
    //逐行解析TXT文件
    while (!file.atEnd()) {
        QByteArray data = file.readLine();
        if (data.isEmpty())
            break;
        
        //移除换行符
        if (data.indexOf('\n') >= 0)
            data.removeLast();
        if (data.indexOf('\r') >= 0)
            data.removeLast();
        
        QString line = QString::fromUtf8(data).trimmed();
        if (line.isEmpty())
            continue;
        
        //跳过无逗号的行
        if (!line.contains(","))
            continue;
        
        //解析行格式："频道名 类型 分辨率(帧率),URL"
        //示例："CCTV1 HD 1920x1080(25),http/udp/239.49.1.1:6000"
        QStringList parts = line.split(",");
        if (parts.length() != 2) {
            errCount++;
            continue;
        }
        
        QString namePart = parts.at(0).trimmed();
        QString url = parts.at(1).trimmed();
        
        //从URL中提取IP和端口
        int i = url.indexOf("udp");
        int j = url.indexOf("rtp");
        if (i < 0 && j < 0) {
            errCount++;
            continue;
        }
        
        int start = (i >= 0 ? i : j) + 4;
        int end = url.indexOf(":", start);
        QString ip = url.mid(start, end - start);
        int port = url.mid(end + 1).toInt();
        
        //解析名称部分："频道名 类型 分辨率(帧率)"
        QStringList nameParts = namePart.split(" ");
        if (nameParts.length() != 3) {
            errCount++;
            continue;
        }
        
        //解析类型
        QString name = nameParts.at(0);
        QString typeStr = nameParts.at(1);
        int type = typeStr == "SD" ? 1 : typeStr == "HD" ? 2 : typeStr == "4K" ? 3 : typeStr == "8K" ? 4 : 0;
        
        //解析分辨率和帧率："1920x1080(25)"
        QStringList resFpsParts = nameParts.at(2).split("(");
        if (resFpsParts.length() != 2) {
            errCount++;
            continue;
        }
        
        QString resPart = resFpsParts.at(0);
        QString fpsStr = resFpsParts.at(1).split(")").at(0);
        
        QStringList resParts = resPart.split("x");
        if (resParts.length() != 2) {
            errCount++;
            continue;
        }
        
        int width = resParts.at(0).toInt();
        int height = resParts.at(1).toInt();
        int fps = fpsStr.toInt();
        
        //获取或创建频道
        int channelId = getOrCreateChannel(name);
        if (channelId < 0) {
            qWarning() << "Failed to create channel:" << name;
            errCount++;
            continue;
        }
        
        //创建信号源
        Database::TvSource source;
        source.tvId = channelId;
        source.ip = ip;
        source.port = port;
        source.width = width;
        source.height = height;
        source.fps = fps;
        source.type = type;
        
        if (addSource(channelId, source)) {
            count++;
        } else {
            errCount++;
        }
    }
    
    file.close();
    qInfo() << "Import completed: success=" << count << " failed=" << errCount;
    return count;
}

QJsonArray ChannelService::buildChannelList(const QString &group,
                                             const Export::ExportOptions &opts)
{
    Q_UNUSED(opts)
    return m_sourceRepo->getTvListByGroup(group, true);
}

QHash<QString, QJsonArray> ChannelService::buildGroupedLists(const QStringList &groups)
{
    QHash<QString, QJsonArray> result;
    
    for (const QString &group : groups) {
        QJsonArray list = m_sourceRepo->getTvListByGroup(group, false);
        if (!list.isEmpty()) {
            result[group] = list;
        }
    }
    
    return result;
}

} // namespace Iptv::Logic
