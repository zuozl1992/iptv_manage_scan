#pragma once

#include <QObject>
#include <QString>

namespace Iptv::Core {

/**
 * @brief 日志管理器
 *        负责日志文件输出和格式化
 */
class LogManager : public QObject
{
    Q_OBJECT

public:
    /** @brief 初始化日志系统 */
    static void init();

    /** @brief 获取当前日志文件路径 */
    static QString logFilePath();

private:
    explicit LogManager(QObject *parent = nullptr);

    /** @brief 自定义消息处理函数 */
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};

} // namespace Iptv::Core
