#pragma once

#include <QObject>
#include <QSettings>
#include <QString>

namespace Iptv::Core {

/**
 * @brief 应用配置管理器（单例）
 *        管理扫描器的所有持久化配置项
 */
class AppConfig : public QObject
{
    Q_OBJECT

public:
    /** @brief 获取单例实例 */
    static AppConfig *instance();

    /** @brief 初始化配置文件路径 */
    void init(const QString &configPath);

    // ==================== 通用访问器 ====================
    QVariant value(const QString &key, const QVariant &defaultValue = {}) const;
    void setValue(const QString &key, const QVariant &value);

    // ==================== 扫描设置 ====================
    /** @brief 获取/设置扫描URL模板 */
    QString scanUrl() const;
    void setScanUrl(const QString &url);

    /** @brief 获取/设置最大并发线程数 */
    int threadMax() const;
    void setThreadMax(int max);

    /** @brief 获取/设置探测超时时间（毫秒） */
    int timeout() const;
    void setTimeout(int ms);

    /** @brief 获取/设置是否自动跳过已有成功IP */
    bool autoStep() const;
    void setAutoStep(bool enabled);

    /** @brief 获取/设置是否使用URL输入模式（false=文件模式） */
    bool useUrl() const;
    void setUseUrl(bool enabled);

    /** @brief 获取/设置模型文件路径 */
    QString modelFilePath() const;
    void setModelFilePath(const QString &path);

    /** @brief 获取/设置是否添加TS前缀 */
    bool addTs() const;
    void setAddTs(bool enabled);

    /** @brief 获取/设置是否启用慢速扫描模式 */
    bool slowScan() const;
    void setSlowScan(bool enabled);

private:
    explicit AppConfig(QObject *parent = nullptr);

    QSettings *m_settings = nullptr;    ///< QSettings实例
    static AppConfig *s_instance;       ///< 单例指针
};

} // namespace Iptv::Core
