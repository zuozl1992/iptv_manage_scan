#include "check_service.h"
#include "multimedia/stream_probe.h"
#include "database/source_repository.h"
#include "database/channel_repository.h"

#include <QDebug>

namespace Iptv::Logic {

CheckService::CheckService(Multimedia::StreamProbe *probe,
                           Database::SourceRepository *srcRepo,
                           Database::ChannelRepository *chRepo,
                           QObject *parent)
    : QObject(parent)
    , m_probe(probe)
    , m_srcRepo(srcRepo)
    , m_chRepo(chRepo)
{
    // Connect probe signals
    connect(m_probe, &Multimedia::StreamProbe::progressChanged,
            this, &CheckService::checkProgress);
    connect(m_probe, &Multimedia::StreamProbe::probeSucceeded,
            this, &CheckService::checkSucceeded);
    connect(m_probe, &Multimedia::StreamProbe::probeFailed,
            this, &CheckService::checkFailed);
}

QJsonArray CheckService::loadCheckList(bool normalType, int order)
{
    return m_srcRepo->getCheckList(normalType, order);
}

void CheckService::startCheck(const QJsonObject &source, const QString &url)
{
    m_probe->probe(url, source);
}

} // namespace Iptv::Logic
