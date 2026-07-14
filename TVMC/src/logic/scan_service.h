#pragma once

#include "multimedia/stream_info.h"
#include <QObject>
#include <QHash>
#include <QStringList>
#include <QSemaphore>
#include <QMutex>
#include <QAtomicInt>
#include <QThread>

namespace Iptv::Multimedia {
class ScannerStreamProbe;
}

namespace Iptv::Logic {

/**
 * @brief 扫描服务
 *        管理多线程并发扫描流程，使用信号量控制并发数
 */
class ScanService : public QObject
{
    Q_OBJECT

public:
    explicit ScanService(QObject *parent = nullptr);

    /** @brief 设置待扫描的URL列表 */
    void setUrlList(const QStringList &urls);

    /** @brief 设置最大并发线程数 */
    void setThreadMax(int max);

    /** @brief 设置探测超时时间（毫秒） */
    void setTimeout(int ms);

    /** @brief 设置是否自动跳过已有成功IP */
    void setAutoStep(bool enabled);

    /** @brief 设置是否启用慢速扫描模式（EIO时减半超时重试） */
    void setSlowScan(bool enabled);

    /** @brief 开始扫描 */
    void start();

    /** @brief 停止扫描 */
    void stop();

    /** @brief 是否正在扫描 */
    bool isRunning() const;

signals:
    /** @brief 扫描进度信号
     *  @param pos        当前位置
     *  @param total      总数
     *  @param okCount    成功数
     *  @param errorCount 失败数 */
    void scanProgress(int pos, int total, int okCount, int errorCount);

    /** @brief 扫描到一个结果 */
    void scanResult(const Multimedia::StreamInfo &info);

    /** @brief 当前扫描URL变化 */
    void currentUrlChanged(const QString &url);

    /** @brief 扫描完成 */
    void scanFinished();

private slots:
    void onProbeSucceeded(const Multimedia::StreamInfo &info);  ///< 探测成功回调
    void onProbeFailed(const QString &error);                   ///< 探测失败回调

private:
    /** @brief 从URL中提取IP地址 */
    QString getIpFromUrl(const QString &url);

    QStringList m_urlList;          ///< 待扫描URL列表
    int m_threadMax = 8;            ///< 最大并发线程数
    int m_timeout = 500;            ///< 探测超时时间（毫秒）
    bool m_autoStep = false;        ///< 是否自动跳过已有成功IP
    bool m_slowScan = false;        ///< 是否启用慢速扫描模式

    QAtomicInt m_pos;               ///< 当前扫描位置
    QAtomicInt m_okCount;           ///< 成功计数
    QAtomicInt m_errorCount;        ///< 失败计数
    QAtomicInt m_activeCount;       ///< 活跃探测线程数
    QAtomicInt m_exitFlag;          ///< 退出标志

    QSemaphore m_semaphore;         ///< 并发控制信号量
    QMutex m_mutex;                 ///< 互斥锁（保护m_probes和m_ipCheck）
    QHash<QString, bool> m_ipCheck; ///< IP去重检查表

    QList<Multimedia::ScannerStreamProbe *> m_probes;  ///< 活跃的探测器列表
};

} // namespace Iptv::Logic
