#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include <QUrl>

namespace Iptv::Network {

/**
 * @brief HTTP客户端
 *        封装Qt网络请求，提供同步和异步HTTP操作
 */
class HttpClient : public QObject
{
    Q_OBJECT

public:
    explicit HttpClient(QObject *parent = nullptr);

    /** @brief 发送GET请求
     *  @param url 目标URL
     *  @return 网络响应对象 */
    QNetworkReply *get(const QUrl &url);

    /** @brief 同步获取图片
     *  @param url 图片URL
     *  @return QPixmap图片对象，失败返回空 */
    QPixmap fetchImageSync(const QUrl &url);

private:
    QNetworkAccessManager *m_nam;   ///< 网络访问管理器
};

} // namespace Iptv::Network
