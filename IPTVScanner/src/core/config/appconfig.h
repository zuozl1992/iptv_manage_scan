#pragma once

#include <QObject>
#include <QSettings>
#include <QString>

namespace Iptv::Core {

class AppConfig : public QObject
{
    Q_OBJECT

public:
    static AppConfig *instance();

    void init(const QString &configPath);

    // 通用访问器
    QVariant value(const QString &key, const QVariant &defaultValue = {}) const;
    void setValue(const QString &key, const QVariant &value);

    // 类型化访问器
    QString scanUrl() const;
    void setScanUrl(const QString &url);

    int threadMax() const;
    void setThreadMax(int max);

    int timeout() const;
    void setTimeout(int ms);

    bool autoStep() const;
    void setAutoStep(bool enabled);

    bool useUrl() const;
    void setUseUrl(bool enabled);

    QString modelFilePath() const;
    void setModelFilePath(const QString &path);

    bool addTs() const;
    void setAddTs(bool enabled);

    bool slowScan() const;
    void setSlowScan(bool enabled);

private:
    explicit AppConfig(QObject *parent = nullptr);

    QSettings *m_settings = nullptr;
    static AppConfig *s_instance;
};

} // namespace Iptv::Core
