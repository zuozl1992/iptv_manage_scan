#pragma once

#include "types.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>

namespace Iptv::Database {

class SourceRepository
{
public:
    explicit SourceRepository(QSqlDatabase *db);

    bool insertSource(const TvSource &source);
    bool updateSource(const TvSource &source);
    bool upsertSource(const TvSource &source);
    
    bool deleteByIp(const QString &ip);
    bool deleteAll();
    
    bool existsByIp(const QString &ip);
    QStringList allIps();
    
    QJsonArray getCheckList(bool normalType, int order);
    QJsonArray getTvListByGroup(const QString &group, bool withLogo);
    
    QStringList getTestUrls(const QString &port);
    
    QSqlQueryModel *createJoinedTableModel(QObject *parent);

private:
    QSqlDatabase *m_db;
};

} // namespace Iptv::Database
