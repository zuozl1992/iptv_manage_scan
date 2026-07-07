#pragma once

#include "stream_info.h"
#include <QThread>
#include <QString>

namespace Iptv::Multimedia {

/**
 * @brief 流媒体探测线程
 *        使用FFmpeg在后台线程中探测视频流的分辨率、帧率等信息
 */
class StreamProbe : public QThread
{
    Q_OBJECT

public:
    explicit StreamProbe(QObject *parent = nullptr);

    /** @brief 设置待探测的URL */
    void setUrl(const QString &url);

    /** @brief 设置探测超时时间（毫秒） */
    void setTimeout(int ms);

    /** @brief 设置是否启用慢速扫描模式 */
    void setSlowScan(bool enabled);

signals:
    /** @brief 探测成功信号
     *  @param info 流媒体信息 */
    void probeSucceeded(const StreamInfo &info);

    /** @brief 探测失败信号
     *  @param error 错误信息 */
    void probeFailed(const QString &error);

protected:
    void run() override;

private:
    QString m_url;              ///< 待探测的URL
    int m_timeout = 500;        ///< 超时时间（毫秒）
    bool m_slowScan = false;    ///< 慢速扫描模式
    int m_retryCount = 2;       ///< 重试次数
};

} // namespace Iptv::Multimedia
