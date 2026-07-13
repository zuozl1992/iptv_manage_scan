#pragma once

#include "types.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QList>
#include <QString>

namespace Iptv::Database {

/**
 * @brief 频道数据仓库
 *        封装tv_info表的所有数据库操作
 */
class ChannelRepository
{
public:
    explicit ChannelRepository(QSqlDatabase *db);

    /** @brief 插入频道记录
     *  @param name      频道名称
     *  @param group     所属分组
     *  @param city      所属城市
     *  @param describe  频道描述
     *  @param notes     备注
     *  @param logoName  台标文件名
     *  @return 新记录的ID，失败返回-1 */
    int insertChannel(const QString &name, const QString &group = {},
                      const QString &city = {}, const QString &describe = {},
                      const QString &notes = {}, const QString &logoName = {});

    /** @brief 按名称查找频道ID
     *  @param name 频道名称
     *  @return 频道ID，不存在返回-1 */
    int findIdByName(const QString &name);

    /** @brief 更新频道信息 */
    bool updateChannel(int id, const TvChannel &channel);

    /** @brief 删除频道 */
    bool deleteChannel(int id);

    /** @brief 删除所有频道 */
    bool deleteAll();

    /** @brief 按ID查找频道 */
    TvChannel findById(int id);

    /** @brief 按分组查找频道列表 */
    QList<TvChannel> findByGroup(const QString &group);

    /** @brief 查找所有频道 */
    QList<TvChannel> findAll();

    /** @brief 创建频道表格模型 */
    QSqlQueryModel *createTableModel(QObject *parent);

private:
    QSqlDatabase *m_db; ///< 数据库连接指针
};

} // namespace Iptv::Database
