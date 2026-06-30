#pragma once

#include "multimedia/stream_info.h"
#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>

namespace Iptv::Multimedia {
class StreamProbe;
}

namespace Iptv::Database {
class SourceRepository;
class ChannelRepository;
}

namespace Iptv::Logic {

class CheckService : public QObject
{
    Q_OBJECT

public:
    explicit CheckService(Multimedia::StreamProbe *probe,
                          Database::SourceRepository *srcRepo,
                          Database::ChannelRepository *chRepo,
                          QObject *parent = nullptr);

    QJsonArray loadCheckList(bool normalType, int order);
    void startCheck(const QJsonObject &source, const QString &url);

signals:
    void checkProgress(int percent);
    void checkSucceeded(const Multimedia::StreamInfo &info,
                        const QJsonObject &original);
    void checkFailed(const QString &error);

private:
    Multimedia::StreamProbe *m_probe;
    Database::SourceRepository *m_srcRepo;
    Database::ChannelRepository *m_chRepo;
};

} // namespace Iptv::Logic
