#include "logmanager.h"
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QDateTime>
#include <QTextStream>
#include <QMutex>
#include <QMutexLocker>

namespace Iptv::Core {

QString LogManager::s_logFilePath;

Q_LOGGING_CATEGORY(lcDb, "iptv.db")
Q_LOGGING_CATEGORY(lcStream, "iptv.stream")
Q_LOGGING_CATEGORY(lcExport, "iptv.export")
Q_LOGGING_CATEGORY(lcNet, "iptv.net")
Q_LOGGING_CATEGORY(lcUi, "iptv.ui")

void LogManager::init(const QString &logDir)
{
    QString dir = logDir;
    if (dir.isEmpty()) {
        dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/logs";
    }
    
    QDir().mkpath(dir);
    
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    s_logFilePath = dir + "/iptv_" + timestamp + ".log";
    
    qInstallMessageHandler(messageHandler);
    
    qInfo() << "Logging initialized:" << s_logFilePath;
}

void LogManager::setLevel(QtMsgType level)
{
    Q_UNUSED(level)
    // Qt logging categories handle filtering
    // This could be extended to set environment variables
}

QString LogManager::logFilePath()
{
    return s_logFilePath;
}

void LogManager::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    
    QString levelStr;
    switch (type) {
    case QtDebugMsg:    levelStr = "DEBUG"; break;
    case QtInfoMsg:     levelStr = "INFO "; break;
    case QtWarningMsg:  levelStr = "WARN "; break;
    case QtCriticalMsg: levelStr = "ERROR"; break;
    case QtFatalMsg:    levelStr = "FATAL"; break;
    }
    
    QString category = context.category ? context.category : "default";
    QString logLine = QString("[%1] [%2] [%3] %4")
        .arg(timestamp)
        .arg(levelStr)
        .arg(category)
        .arg(msg);
    
    // Write to file
    if (!s_logFilePath.isEmpty()) {
        QFile file(s_logFilePath);
        if (file.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << logLine << "\n";
            stream.flush();
        }
    }
    
    // Also write to stderr for debugging
    QTextStream errStream(stderr);
    errStream << logLine << "\n";
    errStream.flush();
    
    if (type == QtFatalMsg) {
        abort();
    }
}

} // namespace Iptv::Core
