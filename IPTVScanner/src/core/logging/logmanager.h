#pragma once

#include <QObject>
#include <QString>

namespace Iptv::Core {

class LogManager : public QObject
{
    Q_OBJECT

public:
    static void init();
    static QString logFilePath();

private:
    explicit LogManager(QObject *parent = nullptr);
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};

} // namespace Iptv::Core
