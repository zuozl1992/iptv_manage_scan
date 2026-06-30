#pragma once

#include <QLoggingCategory>
#include <QString>

namespace Iptv::Core {

class LogManager
{
public:
    static void init(const QString &logDir = QString());
    static void setLevel(QtMsgType level);
    static QString logFilePath();

private:
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    static QString s_logFilePath;
};

} // namespace Iptv::Core

// Logging categories
Q_DECLARE_LOGGING_CATEGORY(lcDb)
Q_DECLARE_LOGGING_CATEGORY(lcStream)
Q_DECLARE_LOGGING_CATEGORY(lcExport)
Q_DECLARE_LOGGING_CATEGORY(lcNet)
Q_DECLARE_LOGGING_CATEGORY(lcUi)
