#pragma once

#include "stream_info.h"
#include <QThread>
#include <QString>

namespace Iptv::Multimedia {

class StreamProbe : public QThread
{
    Q_OBJECT

public:
    explicit StreamProbe(QObject *parent = nullptr);

    void setUrl(const QString &url);
    void setTimeout(int ms);
    void setSlowScan(bool enabled);

signals:
    void probeSucceeded(const StreamInfo &info);
    void probeFailed(const QString &error);

protected:
    void run() override;

private:
    QString m_url;
    int m_timeout = 500;
    bool m_slowScan = false;
    int m_retryCount = 2;
};

} // namespace Iptv::Multimedia
