#pragma once

#include <QSqlDatabase>

namespace Iptv::Database {

/**
 * @brief 数据库模式管理器
 *        负责数据库表结构的创建、版本检查和默认数据初始化
 */
class SchemaManager
{
public:
    /** @brief 确保数据库模式存在且为最新版本
     *  @param db 数据库连接
     *  @return 是否成功 */
    static bool ensureSchema(QSqlDatabase &db);

    /** @brief 获取当前数据库模式版本号
     *  @param db 数据库连接
     *  @return 版本号，失败返回-1 */
    static int currentVersion(QSqlDatabase &db);

    /** @brief 创建默认数据（如信号源类型）
     *  @param db 数据库连接
     *  @return 是否成功 */
    static bool createDefaultData(QSqlDatabase &db);
};

} // namespace Iptv::Database
