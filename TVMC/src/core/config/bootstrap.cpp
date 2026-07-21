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
        appName = "TVMC";
    }
    
    //引导文件存储在应用配置目录
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    
    //如果AppConfigLocation为空，尝试使用AppDataLocation
    if (basePath.isEmpty()) {
        basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }
    
    //如果仍然为空，使用程序所在目录
    if (basePath.isEmpty()) {
        basePath = QCoreApplication::applicationDirPath();
    }
    
    //确保目录存在
    QDir dir(basePath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
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

QString Bootstrap::bootstrapPath() const
{
    return m_bootstrapPath;
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
    
    //检查配置文件和数据库文件是否存在
    return QFile::exists(m_configPath) && QFile::exists(m_dbPath);
}

bool Bootstrap::load()
{
    if (!QFile::exists(m_bootstrapPath)) {
        qInfo() << "Bootstrap file not found:" << m_bootstrapPath;
        return false;
    }
    
    //从引导文件读取路径配置
    QSettings settings(m_bootstrapPath, QSettings::IniFormat);
    m_configPath = settings.value("paths/config").toString();
    m_dbPath = settings.value("paths/db").toString();
    
    //修复Windows路径问题：移除开头的多余的斜杠
    //例如：/D:/path -> D:/path
    if (m_configPath.length() >= 2 && m_configPath[0] == '/' && m_configPath[1].isLetter() && m_configPath[2] == ':') {
        m_configPath = m_configPath.mid(1);
    }
    if (m_dbPath.length() >= 2 && m_dbPath[0] == '/' && m_dbPath[1].isLetter() && m_dbPath[2] == ':') {
        m_dbPath = m_dbPath.mid(1);
    }
    
    qInfo() << "Bootstrap loaded from:" << m_bootstrapPath;
    qInfo() << "Config path:" << m_configPath;
    qInfo() << "DB path:" << m_dbPath;
    
    return isValid();
}

bool Bootstrap::save() const
{
    QDir dir = QFileInfo(m_bootstrapPath).absolutePath();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "Failed to create bootstrap directory:" << dir.absolutePath();
            return false;
        }
    }
    
    //保存路径配置到引导文件
    QSettings settings(m_bootstrapPath, QSettings::IniFormat);
    settings.setValue("paths/config", m_configPath);
    settings.setValue("paths/db", m_dbPath);
    settings.sync();
    
    qInfo() << "Bootstrap saved to:" << m_bootstrapPath;
    qInfo() << "Config path:" << m_configPath;
    qInfo() << "DB path:" << m_dbPath;
    
    return true;
}

} // namespace Iptv::Core
