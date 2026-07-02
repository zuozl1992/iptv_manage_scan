#pragma once

#include "multimedia/stream_info.h"
#include <QObject>
#include <QHash>
#include <QStringList>
#include <QSemaphore>
#include <QMutex>
#include <QAtomicInt>
#include <QThread>

namespace Iptv::Multimedia {
class StreamProbe;
}

namespace Iptv::Logic {

class ScanService : public QObject
{
    Q_OBJECT

public:
    explicit ScanService(QObject *parent = nullptr);

    void setUrlList(const QStringList &urls);
    void setThreadMax(int max);
    void setTimeout(int ms);
    void setAutoStep(bool enabled);
    void setSlowScan(bool enabled);

    void start();
    void stop();
    bool isRunning() const;

signals:
    void scanProgress(int pos, int total, int okCount, int errorCount);
    void scanResult(const Multimedia::StreamInfo &info);
    void currentUrlChanged(const QString &url);
    void scanFinished();

private slots:
    void onProbeSucceeded(const Multimedia::StreamInfo &info);
    void onProbeFailed(const QString &error);

private:
    QString getIpFromUrl(const QString &url);

    QStringList m_urlList;
    int m_threadMax = 8;
    int m_timeout = 500;
    bool m_autoStep = false;
    bool m_slowScan = false;

    QAtomicInt m_pos;
    QAtomicInt m_okCount;
    QAtomicInt m_errorCount;
    QAtomicInt m_activeCount;
    QAtomicInt m_exitFlag;

    QSemaphore m_semaphore;
    QMutex m_mutex;
    QHash<QString, bool> m_ipCheck;

    QList<Multimedia::StreamProbe *> m_probes;
};

} // namespace Iptv::Logic
