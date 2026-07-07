#pragma once

#include <QLoggingCategory>
#include <QString>

namespace Iptv::Core {

/**
 * @brief 日志管理器
 *        负责日志文件输出和日志级别控制
 */
class LogManager
{
public:
    /** @brief 初始化日志系统
     *  @param logDir 日志文件目录（空则使用默认目录） */
    static void init(const QString &logDir = QString());

    /** @brief 设置日志输出级别
     *  @param level 最低日志级别 */
    static void setLevel(QtMsgType level);

    /** @brief 获取当前日志文件路径 */
    static QString logFilePath();

private:
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    static QString s_logFilePath;   ///< 日志文件路径
};

} // namespace Iptv::Core

// ==================== 日志分类 ====================
Q_DECLARE_LOGGING_CATEGORY(lcDb)        ///< 数据库相关日志
Q_DECLARE_LOGGING_CATEGORY(lcStream)    ///< 流媒体相关日志
Q_DECLARE_LOGGING_CATEGORY(lcExport)    ///< 导出相关日志
Q_DECLARE_LOGGING_CATEGORY(lcNet)       ///< 网络相关日志
Q_DECLARE_LOGGING_CATEGORY(lcUi)        ///< 界面相关日志
