#include "appconfig.h"
#include "configkeys.h"

#include <QDir>
#include <QFileInfo>

namespace Iptv::Core {

AppConfig *AppConfig::s_instance = nullptr;

AppConfig *AppConfig::instance()
{
    //单例模式：延迟初始化
    if (!s_instance) {
        s_instance = new AppConfig();
    }
    return s_instance;
}

AppConfig::AppConfig(QObject *parent)
    : QObject(parent)
    , m_settings(nullptr)
{
}

void AppConfig::init(const QString &configPath)
{
    if (m_settings) {
        delete m_settings;
    }
    //确保配置目录存在
    QFileInfo fileInfo(configPath);
    QDir().mkpath(fileInfo.absolutePath());
    m_settings = new QSettings(configPath, QSettings::IniFormat, this);
}

QVariant AppConfig::value(const QString &key, const QVariant &defaultValue) const
{
    if (!m_settings) return defaultValue;
    return m_settings->value(key, defaultValue);
}

void AppConfig::setValue(const QString &key, const QVariant &value)
{
    if (!m_settings) return;
    m_settings->setValue(key, value);
    m_settings->sync();
}

QString AppConfig::scanUrl() const
{
    return value(ConfigKeys::SCAN_URL, QString()).toString();
}

void AppConfig::setScanUrl(const QString &url)
{
    setValue(ConfigKeys::SCAN_URL, url);
}

int AppConfig::threadMax() const
{
    return value(ConfigKeys::THREAD_MAX, ConfigKeys::DEFAULT_THREAD_MAX).toInt();
}

void AppConfig::setThreadMax(int max)
{
    setValue(ConfigKeys::THREAD_MAX, max);
}

int AppConfig::timeout() const
{
    return value(ConfigKeys::TIMEOUT, ConfigKeys::DEFAULT_TIMEOUT).toInt();
}

void AppConfig::setTimeout(int ms)
{
    setValue(ConfigKeys::TIMEOUT, ms);
}

bool AppConfig::autoStep() const
{
    return value(ConfigKeys::AUTO_STEP, ConfigKeys::DEFAULT_AUTO_STEP).toBool();
}

void AppConfig::setAutoStep(bool enabled)
{
    setValue(ConfigKeys::AUTO_STEP, enabled);
}

bool AppConfig::useUrl() const
{
    return value(ConfigKeys::USE_URL, ConfigKeys::DEFAULT_USE_URL).toBool();
}

void AppConfig::setUseUrl(bool enabled)
{
    setValue(ConfigKeys::USE_URL, enabled);
}

QString AppConfig::modelFilePath() const
{
    return value(ConfigKeys::MODEL_FILE_PATH, QString()).toString();
}

void AppConfig::setModelFilePath(const QString &path)
{
    setValue(ConfigKeys::MODEL_FILE_PATH, path);
}

bool AppConfig::addTs() const
{
    return value(ConfigKeys::ADD_TS, ConfigKeys::DEFAULT_ADD_TS).toBool();
}

void AppConfig::setAddTs(bool enabled)
{
    setValue(ConfigKeys::ADD_TS, enabled);
}

bool AppConfig::slowScan() const
{
    return value(ConfigKeys::SLOW_SCAN, ConfigKeys::DEFAULT_SLOW_SCAN).toBool();
}

void AppConfig::setSlowScan(bool enabled)
{
    setValue(ConfigKeys::SLOW_SCAN, enabled);
}

} // namespace Iptv::Core
