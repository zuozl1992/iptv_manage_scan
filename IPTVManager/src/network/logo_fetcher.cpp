#include "logo_fetcher.h"
#include "httpclient.h"
#include <QNetworkReply>

namespace Iptv::Network {

LogoFetcher::LogoFetcher(HttpClient *client, QObject *parent)
    : QObject(parent)
    , m_client(client)
{
}

void LogoFetcher::fetch(const QUrl &url)
{
    QNetworkReply *reply = m_client->get(url);
    
    connect(reply, &QNetworkReply::finished, this, &LogoFetcher::onReplyFinished);
}

void LogoFetcher::onReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    if (reply->error() == QNetworkReply::NoError) {
        QPixmap pixmap;
        //从网络数据加载图片
        if (pixmap.loadFromData(reply->readAll())) {
            emit imageReady(pixmap);
        } else {
            emit fetchFailed(tr("Failed to load image data"));
        }
    } else {
        emit fetchFailed(reply->errorString());
    }
    
    reply->deleteLater();
}

} // namespace Iptv::Network
