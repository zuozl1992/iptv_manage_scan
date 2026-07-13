#pragma once

#include "stream_info.h"
#include <QThread>
#include <QJsonObject>
#include <QString>

namespace Iptv::Multimedia {

/**
 * @brief 流媒体探测线程
 *        在后台线程中探测视频流信息，包括分辨率、帧率、编码格式等
 */
class StreamProbe : public QThread
{
    Q_OBJECT

public:
    explicit StreamProbe(QObject *parent = nullptr);

    /** @brief 开始探测指定URL的流信息
     *  @param url        流媒体URL
     *  @param sourceInfo 原始信号源信息（透传回调） */
    void probe(const QString &url, const QJsonObject &sourceInfo);

signals:
    /** @brief 探测进度信号
     *  @param percent 进度百分比（0-100） */
    void progressChanged(int percent);

    /** @brief 探测成功信号
     *  @param info           流媒体信息
     *  @param originalSource 原始信号源数据 */
    void probeSucceeded(const StreamInfo &info, const QJsonObject &originalSource);

    /** @brief 探测失败信号
     *  @param error 错误信息 */
    void probeFailed(const QString &error);

protected:
    void run() override;

private:
    QString m_url;              ///< 待探测的流媒体URL
    QJsonObject m_sourceInfo;   ///< 原始信号源信息
};

} // namespace Iptv::Multimedia
