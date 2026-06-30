#include "bootstrap.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QCoreApplication>

namespace Iptv::Core {

Bootstrap::Bootstrap()
{
    QString appName = QCoreApplication::applicationName();
    if (appName.isEmpty()) {
        appName = "IPTVManager";
    }
    
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    m_bootstrapPath = basePath + "/" + appName + "_iptv_paths.ini";
}

QString Bootstrap::configPath() const
{
    return m_configPath;
}

QString Bootstrap::dbPath() const
{
    return m_dbPath;
}

void Bootstrap::setConfigPath(const QString &path)
{
    m_configPath = path;
}

void Bootstrap::setDbPath(const QString &path)
{
    m_dbPath = path;
}

bool Bootstrap::isValid() const
{
    if (m_configPath.isEmpty() || m_dbPath.isEmpty()) {
        return false;
    }
    
    return QFile::exists(m_configPath) && QFile::exists(m_dbPath);
}

bool Bootstrap::load()
{
    if (!QFile::exists(m_bootstrapPath)) {
        return false;
    }
    
    QSettings settings(m_bootstrapPath, QSettings::IniFormat);
    m_configPath = settings.value("paths/config").toString();
    m_dbPath = settings.value("paths/db").toString();
    
    return isValid();
}

bool Bootstrap::save() const
{
    QDir dir = QFileInfo(m_bootstrapPath).absolutePath();
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    QSettings settings(m_bootstrapPath, QSettings::IniFormat);
    settings.setValue("paths/config", m_configPath);
    settings.setValue("paths/db", m_dbPath);
    
    return true;
}

} // namespace Iptv::Core
