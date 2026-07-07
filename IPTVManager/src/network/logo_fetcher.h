#pragma once

#include <QObject>
#include <QPixmap>
#include <QUrl>

namespace Iptv::Network {

class HttpClient;

/**
 * @brief 台标图片获取器
 *        异步下载频道台标图片
 */
class LogoFetcher : public QObject
{
    Q_OBJECT

public:
    explicit LogoFetcher(HttpClient *client, QObject *parent = nullptr);

    /** @brief 异步获取台标图片
     *  @param url 台标图片URL */
    void fetch(const QUrl &url);

signals:
    /** @brief 图片获取成功信号
     *  @param pixmap 获取到的图片 */
    void imageReady(const QPixmap &pixmap);

    /** @brief 图片获取失败信号
     *  @param error 错误信息 */
    void fetchFailed(const QString &error);

private slots:
    void onReplyFinished();

private:
    HttpClient *m_client;   ///< HTTP客户端
};

} // namespace Iptv::Network
