#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QString>

namespace Iptv::Database {

/**
 * @brief 数据库管理器
 *        负责SQLite数据库的打开、关闭和重置操作
 */
class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(const QString &dbPath, QObject *parent = nullptr);
    ~DatabaseManager();

    /** @brief 打开数据库连接
     *  @return 是否打开成功 */
    bool open();

    /** @brief 关闭数据库连接 */
    void close();

    /** @brief 检查数据库是否已打开 */
    bool isOpen() const;

    /** @brief 重置为默认数据库路径 */
    bool resetToDefault();

    /** @brief 获取数据库连接指针 */
    QSqlDatabase *database();

    /** @brief 获取数据库文件路径 */
    QString dbPath() const;

private:
    QString m_dbPath;       ///< 数据库文件路径
    QSqlDatabase m_db;      ///< SQLite数据库连接
};

} // namespace Iptv::Database
