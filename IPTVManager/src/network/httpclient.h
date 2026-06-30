#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include <QUrl>

namespace Iptv::Network {

class HttpClient : public QObject
{
    Q_OBJECT

public:
    explicit HttpClient(QObject *parent = nullptr);

    QNetworkReply *get(const QUrl &url);
    QPixmap fetchImageSync(const QUrl &url);

private:
    QNetworkAccessManager *m_nam;
};

} // namespace Iptv::Network
