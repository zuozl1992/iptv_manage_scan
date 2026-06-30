#pragma once

#include <QObject>
#include <QPixmap>
#include <QUrl>

namespace Iptv::Network {

class HttpClient;

class LogoFetcher : public QObject
{
    Q_OBJECT

public:
    explicit LogoFetcher(HttpClient *client, QObject *parent = nullptr);

    void fetch(const QUrl &url);

signals:
    void imageReady(const QPixmap &pixmap);
    void fetchFailed(const QString &error);

private slots:
    void onReplyFinished();

private:
    HttpClient *m_client;
};

} // namespace Iptv::Network
