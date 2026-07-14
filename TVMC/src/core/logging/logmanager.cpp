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

//定义日志分类
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
    
    //使用时间戳生成日志文件名
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    s_logFilePath = dir + "/iptv_" + timestamp + ".log";
    
    //安装自定义消息处理器
    qInstallMessageHandler(messageHandler);
    
    qInfo() << "Logging initialized:" << s_logFilePath;
}

void LogManager::setLevel(QtMsgType level)
{
    Q_UNUSED(level)
    //Qt日志分类处理过滤
}

QString LogManager::logFilePath()
{
    return s_logFilePath;
}

void LogManager::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    //线程安全的日志写入
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    
    //格式化日志级别
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
    
    //写入日志文件
    if (!s_logFilePath.isEmpty()) {
        QFile file(s_logFilePath);
        if (file.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << logLine << "\n";
            stream.flush();
        }
    }
    
    //同时输出到标准错误流
    QTextStream errStream(stderr);
    errStream << logLine << "\n";
    errStream.flush();
    
    if (type == QtFatalMsg) {
        abort();
    }
}

} // namespace Iptv::Core
