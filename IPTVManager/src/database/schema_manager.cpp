#include "schema_manager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

namespace Iptv::Database {

bool SchemaManager::ensureSchema(QSqlDatabase &db)
{
    QSqlQuery query(db);
    
    //创建信号源类型表
    if (!query.exec(
        "CREATE TABLE IF NOT EXISTS source_type ("
        "  id INTEGER NOT NULL PRIMARY KEY,"
        "  name VARCHAR(255) NOT NULL"
        ")")) {
        qCritical() << "Failed to create source_type table:" << query.lastError().text();
        return false;
    }
    
    //创建频道信息表
    if (!query.exec(
        "CREATE TABLE IF NOT EXISTS tv_info ("
        "  id INTEGER NOT NULL PRIMARY KEY,"
        "  channel_id INTEGER DEFAULT NULL,"
        "  name VARCHAR(255) NOT NULL,"
        "  \"group\" VARCHAR(255) DEFAULT NULL,"
        "  city VARCHAR(255) DEFAULT NULL,"
        "  describe TEXT,"
        "  notes TEXT,"
        "  logo_name VARCHAR(255) DEFAULT NULL,"
        "  UNIQUE(name ASC)"
        ")")) {
        qCritical() << "Failed to create tv_info table:" << query.lastError().text();
        return false;
    }
    
    //创建信号源信息表
    if (!query.exec(
        "CREATE TABLE IF NOT EXISTS tv_source_info ("
        "  id INTEGER NOT NULL PRIMARY KEY,"
        "  tv_id INTEGER NOT NULL,"
        "  ip VARCHAR(255) DEFAULT NULL,"
        "  port INTEGER DEFAULT NULL,"
        "  width INTEGER NOT NULL DEFAULT 0,"
        "  height INTEGER NOT NULL DEFAULT 0,"
        "  fps INTEGER NOT NULL DEFAULT 0,"
        "  last_check_date DATETIME NOT NULL,"
        "  type INTEGER NOT NULL DEFAULT 0,"
        "  notes TEXT,"
        "  CONSTRAINT tv_source_info_ibfk_1 FOREIGN KEY (tv_id) REFERENCES tv_info (id) "
        "    ON DELETE CASCADE ON UPDATE CASCADE,"
        "  CONSTRAINT tv_source_info_ibfk_2 FOREIGN KEY (type) REFERENCES source_type (id) "
        "    ON DELETE RESTRICT ON UPDATE RESTRICT,"
        "  UNIQUE(ip ASC)"
        ")")) {
        qCritical() << "Failed to create tv_source_info table:" << query.lastError().text();
        return false;
    }
    
    //检查是否需要插入默认信号源类型数据
    query.exec("SELECT COUNT(*) FROM source_type");
    if (query.next() && query.value(0).toInt() == 0) {
        createDefaultData(db);
    }
    
    return true;
}

int SchemaManager::currentVersion(QSqlDatabase &db)
{
    Q_UNUSED(db)
    return 1;  //当前模式版本
}

bool SchemaManager::createDefaultData(QSqlDatabase &db)
{
    QSqlQuery query(db);
    
    //插入默认信号源类型
    QStringList types = {"TS", "SD", "HD", "4K", "8K"};
    for (int i = 0; i < types.size(); ++i) {
        query.prepare("INSERT INTO source_type (id, name) VALUES (:id, :name)");
        query.bindValue(":id", i);
        query.bindValue(":name", types[i]);
        if (!query.exec()) {
            qWarning() << "Failed to insert source type:" << query.lastError().text();
        }
    }
    
    return true;
}

} // namespace Iptv::Database
