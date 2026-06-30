#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QString>

namespace Iptv::Database {

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(const QString &dbPath, QObject *parent = nullptr);
    ~DatabaseManager();

    bool open();
    void close();
    bool isOpen() const;
    
    bool resetToDefault();
    
    QSqlDatabase *database();
    QString dbPath() const;

private:
    QString m_dbPath;
    QSqlDatabase m_db;
};

} // namespace Iptv::Database
