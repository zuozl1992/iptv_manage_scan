#include "channel_repository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

namespace Iptv::Database {

ChannelRepository::ChannelRepository(QSqlDatabase *db)
    : m_db(db)
{
}

int ChannelRepository::insertChannel(const QString &name, const QString &group,
                                      const QString &city, const QString &describe,
                                      const QString &notes, const QString &logoName)
{
    QSqlQuery query(*m_db);
    //自动生成ID并插入频道记录
    query.prepare(
        "INSERT INTO tv_info (id, name, \"group\", city, describe, notes, logo_name) "
        "VALUES (COALESCE((SELECT MAX(id)+1 FROM tv_info), 0), :name, :group, :city, :describe, :notes, :logo_name)");
    query.bindValue(":name", name);
    query.bindValue(":group", group);
    query.bindValue(":city", city);
    query.bindValue(":describe", describe);
    query.bindValue(":notes", notes);
    query.bindValue(":logo_name", logoName);
    
    if (!query.exec()) {
        qWarning() << "Failed to insert channel:" << query.lastError().text();
        return -1;
    }
    
    return query.lastInsertId().toInt();
}

int ChannelRepository::findIdByName(const QString &name)
{
    QSqlQuery query(*m_db);
    query.prepare("SELECT id FROM tv_info WHERE name = :name");
    query.bindValue(":name", name);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    
    return -1;
}

bool ChannelRepository::updateChannel(int id, const TvChannel &channel)
{
    QSqlQuery query(*m_db);
    query.prepare(
        "UPDATE tv_info SET channel_id = :channel_id, name = :name, \"group\" = :group, "
        "city = :city, describe = :describe, notes = :notes, logo_name = :logo_name "
        "WHERE id = :id");
    query.bindValue(":channel_id", channel.channelId);
    query.bindValue(":name", channel.name);
    query.bindValue(":group", channel.group);
    query.bindValue(":city", channel.city);
    query.bindValue(":describe", channel.describe);
    query.bindValue(":notes", channel.notes);
    query.bindValue(":logo_name", channel.logoName);
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qWarning() << "Failed to update channel:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool ChannelRepository::deleteChannel(int id)
{
    QSqlQuery query(*m_db);
    query.prepare("DELETE FROM tv_info WHERE id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qWarning() << "Failed to delete channel:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool ChannelRepository::deleteAll()
{
    QSqlQuery query(*m_db);
    if (!query.exec("DELETE FROM tv_info")) {
        qWarning() << "Failed to delete all channels:" << query.lastError().text();
        return false;
    }
    
    return true;
}

TvChannel ChannelRepository::findById(int id)
{
    QSqlQuery query(*m_db);
    query.prepare("SELECT id, channel_id, name, \"group\", city, describe, notes, logo_name FROM tv_info WHERE id = :id");
    query.bindValue(":id", id);
    
    TvChannel channel;
    if (query.exec() && query.next()) {
        channel.id = query.value(0).toInt();
        channel.channelId = query.value(1).toInt();
        channel.name = query.value(2).toString();
        channel.group = query.value(3).toString();
        channel.city = query.value(4).toString();
        channel.describe = query.value(5).toString();
        channel.notes = query.value(6).toString();
        channel.logoName = query.value(7).toString();
    }
    
    return channel;
}

QList<TvChannel> ChannelRepository::findByGroup(const QString &group)
{
    QSqlQuery query(*m_db);
    query.prepare("SELECT id, channel_id, name, \"group\", city, describe, notes, logo_name FROM tv_info WHERE \"group\" = :group ORDER BY channel_id");
    query.bindValue(":group", group);
    
    QList<TvChannel> channels;
    if (query.exec()) {
        while (query.next()) {
            TvChannel channel;
            channel.id = query.value(0).toInt();
            channel.channelId = query.value(1).toInt();
            channel.name = query.value(2).toString();
            channel.group = query.value(3).toString();
            channel.city = query.value(4).toString();
            channel.describe = query.value(5).toString();
            channel.notes = query.value(6).toString();
            channel.logoName = query.value(7).toString();
            channels.append(channel);
        }
    }
    
    return channels;
}

QList<TvChannel> ChannelRepository::findAll()
{
    QSqlQuery query(*m_db);
    if (!query.exec("SELECT id, channel_id, name, \"group\", city, describe, notes, logo_name FROM tv_info ORDER BY channel_id")) {
        return {};
    }
    
    QList<TvChannel> channels;
    while (query.next()) {
        TvChannel channel;
        channel.id = query.value(0).toInt();
        channel.channelId = query.value(1).toInt();
        channel.name = query.value(2).toString();
        channel.group = query.value(3).toString();
        channel.city = query.value(4).toString();
        channel.describe = query.value(5).toString();
        channel.notes = query.value(6).toString();
        channel.logoName = query.value(7).toString();
        channels.append(channel);
    }
    
    return channels;
}

QSqlQueryModel *ChannelRepository::createTableModel(QObject *parent)
{
    QSqlQueryModel *model = new QSqlQueryModel(parent);
    model->setQuery("SELECT id, channel_id, name, \"group\", city, describe, notes, logo_name FROM tv_info ORDER BY channel_id", *m_db);
    
    //设置中文列标题
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "频道");
    model->setHeaderData(2, Qt::Horizontal, "分组");
    model->setHeaderData(3, Qt::Horizontal, "城市");
    model->setHeaderData(4, Qt::Horizontal, "描述");
    model->setHeaderData(5, Qt::Horizontal, "备注");
    model->setHeaderData(6, Qt::Horizontal, "Logo");
    
    return model;
}

} // namespace Iptv::Database
