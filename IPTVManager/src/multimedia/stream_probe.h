#pragma once

#include "stream_info.h"
#include <QThread>
#include <QJsonObject>
#include <QString>

namespace Iptv::Multimedia {

class StreamProbe : public QThread
{
    Q_OBJECT

public:
    explicit StreamProbe(QObject *parent = nullptr);

    void probe(const QString &url, const QJsonObject &sourceInfo);

signals:
    void progressChanged(int percent);
    void probeSucceeded(const StreamInfo &info, const QJsonObject &originalSource);
    void probeFailed(const QString &error);

protected:
    void run() override;

private:
    QString m_url;
    QJsonObject m_sourceInfo;
};

} // namespace Iptv::Multimedia
