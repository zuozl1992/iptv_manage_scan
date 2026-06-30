#include "httpclient.h"
#include <QEventLoop>
#include <QNetworkRequest>

namespace Iptv::Network {

HttpClient::HttpClient(QObject *parent)
    : QObject(parent)
    , m_nam(new QNetworkAccessManager(this))
{
}

QNetworkReply *HttpClient::get(const QUrl &url)
{
    QNetworkRequest request(url);
    return m_nam->get(request);
}

QPixmap HttpClient::fetchImageSync(const QUrl &url)
{
    QNetworkReply *reply = get(url);
    
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    
    QPixmap pixmap;
    if (reply->error() == QNetworkReply::NoError) {
        pixmap.loadFromData(reply->readAll());
    }
    
    reply->deleteLater();
    return pixmap;
}

} // namespace Iptv::Network
