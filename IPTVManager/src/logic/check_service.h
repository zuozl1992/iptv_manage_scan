#pragma once

#include "multimedia/stream_info.h"
#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>

namespace Iptv::Multimedia {
class StreamProbe;
}

namespace Iptv::Database {
class SourceRepository;
class ChannelRepository;
}

namespace Iptv::Logic {

/**
 * @brief 流检测业务服务类
 *        管理信号源的流媒体检测流程，包括加载检测列表和执行检测
 */
class CheckService : public QObject
{
    Q_OBJECT

public:
    explicit CheckService(Multimedia::StreamProbe *probe,
                          Database::SourceRepository *srcRepo,
                          Database::ChannelRepository *chRepo,
                          QObject *parent = nullptr);

    /** @brief 加载检测列表
     *  @param normalType true=正常源，false=异常源（type=0）
     *  @param order      排序方式（0=按IP，1=按分辨率）
     *  @return 检测项JSON数组 */
    QJsonArray loadCheckList(bool normalType, int order);

    /** @brief 开始检测指定信号源
     *  @param source 信号源信息JSON对象
     *  @param url    完整的流媒体URL */
    void startCheck(const QJsonObject &source, const QString &url);

signals:
    /** @brief 检测进度信号
     *  @param percent 进度百分比（0-100） */
    void checkProgress(int percent);

    /** @brief 检测成功信号
     *  @param info     流媒体信息
     *  @param original 原始信号源数据 */
    void checkSucceeded(const Multimedia::StreamInfo &info,
                        const QJsonObject &original);

    /** @brief 检测失败信号
     *  @param error 错误信息 */
    void checkFailed(const QString &error);

private:
    Multimedia::StreamProbe *m_probe;           ///< 流探测器
    Database::SourceRepository *m_srcRepo;      ///< 信号源数据仓库
    Database::ChannelRepository *m_chRepo;      ///< 频道数据仓库
};

} // namespace Iptv::Logic
