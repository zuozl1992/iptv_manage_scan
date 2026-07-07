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

//定义日志分类
Q_LOGGING_CATEGORY(lcScan, "iptv.scanner.scan")
Q_LOGGING_CATEGORY(lcNet, "iptv.scanner.net")

static QFile s_logFile;
static QMutex s_mutex;

void LogManager::init()
{
    //创建日志目录
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/logs";
    QDir().mkpath(logDir);

    //使用日期生成日志文件名
    QString logPath = logDir + "/IPTVScanner_" + QDateTime::currentDateTime().toString("yyyyMMdd") + ".log";
    s_logFile.setFileName(logPath);
    s_logFile.open(QIODevice::WriteOnly | QIODevice::Append);

    //安装自定义消息处理器
    qInstallMessageHandler(messageHandler);
    qInfo() << "Logging initialized:" << logPath;
}

QString LogManager::logFilePath()
{
    return s_logFile.fileName();
}

void LogManager::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    //线程安全的日志写入
    QMutexLocker locker(&s_mutex);

    //格式化日志级别
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

    //写入日志文件
    if (s_logFile.isOpen()) {
        QTextStream stream(&s_logFile);
        stream << logMsg;
        stream.flush();
    }

    //同时输出到标准错误流
    QByteArray logData = logMsg.toUtf8();
    fwrite(logData.constData(), 1, logData.size(), stderr);
}

LogManager::LogManager(QObject *parent)
    : QObject(parent)
{
}

} // namespace Iptv::Core
