#include "logmanager.h"
#include "logcategories.h"

#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include <cstdio>

namespace Iptv::Core {

Q_LOGGING_CATEGORY(lcScan, "iptv.scanner.scan")
Q_LOGGING_CATEGORY(lcNet, "iptv.scanner.net")

static QFile s_logFile;
static QMutex s_mutex;

void LogManager::init()
{
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/logs";
    QDir().mkpath(logDir);

    QString logPath = logDir + "/IPTVScanner_" + QDateTime::currentDateTime().toString("yyyyMMdd") + ".log";
    s_logFile.setFileName(logPath);
    s_logFile.open(QIODevice::WriteOnly | QIODevice::Append);

    qInstallMessageHandler(messageHandler);
    qInfo() << "Logging initialized:" << logPath;
}

QString LogManager::logFilePath()
{
    return s_logFile.fileName();
}

void LogManager::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QMutexLocker locker(&s_mutex);

    QString level;
    switch (type) {
    case QtDebugMsg: level = "DEBUG"; break;
    case QtInfoMsg: level = "INFO"; break;
    case QtWarningMsg: level = "WARN"; break;
    case QtCriticalMsg: level = "CRITICAL"; break;
    case QtFatalMsg: level = "FATAL"; break;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString category = context.category ? context.category : "default";
    QString logMsg = QString("[%1] [%2] [%3] %4\n")
        .arg(timestamp, level, category, msg);

    if (s_logFile.isOpen()) {
        QTextStream stream(&s_logFile);
        stream << logMsg;
        stream.flush();
    }

    // Also output to stderr for debugging
    QByteArray logData = logMsg.toUtf8();
    fwrite(logData.constData(), 1, logData.size(), stderr);
}

LogManager::LogManager(QObject *parent)
    : QObject(parent)
{
}

} // namespace Iptv::Core
