#pragma once

#include <QObject>
#include <QJsonArray>
#include <QString>
#include <QHash>

namespace Iptv::Database {
class ChannelRepository;
class SourceRepository;
struct TvSource;
}

namespace Iptv::Export {
struct ExportOptions;
}

namespace Iptv::Logic {

class ChannelService : public QObject
{
    Q_OBJECT

public:
    explicit ChannelService(Database::ChannelRepository *channelRepo,
                            Database::SourceRepository *sourceRepo,
                            QObject *parent = nullptr);

    int addChannel(const QString &name);
    int getOrCreateChannel(const QString &name);
    bool addSource(int tvId, const Database::TvSource &source);
    bool removeSource(const QString &ip);
    bool removeAllSources();
    bool removeAllChannels();
    
    int importFromTxt(const QString &filePath);
    
    QJsonArray buildChannelList(const QString &group,
                                const Export::ExportOptions &opts);
    QHash<QString, QJsonArray> buildGroupedLists(const QStringList &groups);

private:
    Database::ChannelRepository *m_channelRepo;
    Database::SourceRepository *m_sourceRepo;
};

} // namespace Iptv::Logic
