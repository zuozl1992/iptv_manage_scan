#pragma once

#include "types.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QList>
#include <QString>

namespace Iptv::Database {

class ChannelRepository
{
public:
    explicit ChannelRepository(QSqlDatabase *db);

    int insertChannel(const QString &name, const QString &group = {},
                      const QString &city = {}, const QString &describe = {},
                      const QString &notes = {}, const QString &logoName = {});
    
    int findIdByName(const QString &name);
    
    bool updateChannel(int id, const TvChannel &channel);
    bool deleteChannel(int id);
    bool deleteAll();
    
    TvChannel findById(int id);
    QList<TvChannel> findByGroup(const QString &group);
    QList<TvChannel> findAll();
    
    QSqlQueryModel *createTableModel(QObject *parent);

private:
    QSqlDatabase *m_db;
};

} // namespace Iptv::Database
