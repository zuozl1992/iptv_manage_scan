#include "scan_service.h"
#include "multimedia/stream_probe.h"
#include "url_builder.h"

#include <QDebug>
#include <QCoreApplication>

namespace Iptv::Logic {

ScanService::ScanService(QObject *parent)
    : QObject(parent)
{
}

void ScanService::setUrlList(const QStringList &urls)
{
    m_urlList = urls;
}

void ScanService::setThreadMax(int max)
{
    m_threadMax = max;
}

void ScanService::setTimeout(int ms)
{
    m_timeout = ms;
}

void ScanService::setAutoStep(bool enabled)
{
    m_autoStep = enabled;
}

void ScanService::setSlowScan(bool enabled)
{
    m_slowScan = enabled;
}

void ScanService::start()
{
    if (isRunning())
        return;

    m_pos = 0;
    m_okCount = 0;
    m_errorCount = 0;
    m_activeCount = 0;
    m_exitFlag = 0;
    m_ipCheck.clear();
    m_semaphore.release(m_threadMax);

    // Process URLs in a separate thread to avoid blocking the UI
    QThread::create([this]() {
        for (int i = 0; i < m_urlList.length(); i++) {
            if ((int)m_exitFlag)
                break;

            m_semaphore.acquire();

            if ((int)m_exitFlag) {
                m_semaphore.release();
                break;
            }

            QString url = m_urlList.at(i);
            emit currentUrlChanged(url);

            if (m_autoStep) {
                QString ip = getIpFromUrl(url);
                bool step = m_ipCheck.value(ip, false);
                if (step) {
                    m_pos++;
                    m_errorCount++;
                    emit scanProgress((int)m_pos, m_urlList.length(),
                                     (int)m_okCount, (int)m_errorCount);
                    m_semaphore.release();
                    continue;
                }
            }

            Multimedia::StreamProbe *probe = new Multimedia::StreamProbe();
            probe->setUrl(url);
            probe->setTimeout(m_timeout);
            probe->setSlowScan(m_slowScan);

            connect(probe, &Multimedia::StreamProbe::probeSucceeded,
                    this, &ScanService::onProbeSucceeded);
            connect(probe, &Multimedia::StreamProbe::probeFailed,
                    this, &ScanService::onProbeFailed);

            m_mutex.lock();
            m_probes.append(probe);
            m_mutex.unlock();

            m_activeCount++;
            probe->start();
        }

        // Wait for all active probes to finish
        while ((int)m_activeCount > 0) {
            QThread::msleep(10);
        }

        emit scanFinished();
    })->start();
}

void ScanService::stop()
{
    m_exitFlag = 1;

    m_mutex.lock();
    for (auto *probe : m_probes) {
        probe->quit();
        probe->wait(1000);
    }
    m_mutex.unlock();
}

bool ScanService::isRunning() const
{
    return (int)m_activeCount > 0;
}

void ScanService::onProbeSucceeded(const Multimedia::StreamInfo &info)
{
    Multimedia::StreamProbe *probe = qobject_cast<Multimedia::StreamProbe *>(sender());
    if (!probe)
        return;

    QString ip = getIpFromUrl(info.url);
    m_ipCheck.insert(ip, true);

    m_pos++;
    m_okCount++;

    emit scanResult(info);
    emit scanProgress((int)m_pos, m_urlList.length(),
                     (int)m_okCount, (int)m_errorCount);

    m_mutex.lock();
    m_probes.removeOne(probe);
    m_mutex.unlock();

    m_activeCount--;
    m_semaphore.release();

    probe->deleteLater();
}

void ScanService::onProbeFailed(const QString &error)
{
    Q_UNUSED(error)

    Multimedia::StreamProbe *probe = qobject_cast<Multimedia::StreamProbe *>(sender());
    if (!probe)
        return;

    m_pos++;
    m_errorCount++;

    emit scanProgress((int)m_pos, m_urlList.length(),
                     (int)m_okCount, (int)m_errorCount);

    m_mutex.lock();
    m_probes.removeOne(probe);
    m_mutex.unlock();

    m_activeCount--;
    m_semaphore.release();

    probe->deleteLater();
}

QString ScanService::getIpFromUrl(const QString &url)
{
    return UrlBuilder::getIpFromUrl(url);
}

} // namespace Iptv::Logic
