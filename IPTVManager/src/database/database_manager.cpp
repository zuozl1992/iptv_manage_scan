#include "database_manager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

namespace Iptv::Database {

DatabaseManager::DatabaseManager(const QString &dbPath, QObject *parent)
    : QObject(parent)
    , m_dbPath(dbPath)
{
}

DatabaseManager::~DatabaseManager()
{
    close();
}

bool DatabaseManager::open()
{
    if (m_db.isOpen()) {
        return true;
    }
    
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_dbPath);
    
    if (!m_db.open()) {
        qCritical() << "Failed to open database:" << m_db.lastError().text();
        return false;
    }
    
    // Enable foreign keys
    QSqlQuery query(m_db);
    query.exec("PRAGMA foreign_keys = ON");
    
    qInfo() << "Database opened:" << m_dbPath;
    return true;
}

void DatabaseManager::close()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::isOpen() const
{
    return m_db.isOpen();
}

bool DatabaseManager::resetToDefault()
{
    close();
    
    // Remove existing database
    if (QFile::exists(m_dbPath)) {
        QFile::remove(m_dbPath);
    }
    
    // Copy default database from resources
    QString defaultDb = ":/db_def.db";
    if (!QFile::copy(defaultDb, m_dbPath)) {
        qCritical() << "Failed to copy default database";
        return false;
    }
    
    // Set permissions
    QFile::setPermissions(m_dbPath, QFile::ReadOwner | QFile::WriteOwner);
    
    // Reopen
    return open();
}

QSqlDatabase *DatabaseManager::database()
{
    return &m_db;
}

QString DatabaseManager::dbPath() const
{
    return m_dbPath;
}

} // namespace Iptv::Database
