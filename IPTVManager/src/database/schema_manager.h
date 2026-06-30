#pragma once

#include <QSqlDatabase>

namespace Iptv::Database {

class SchemaManager
{
public:
    static bool ensureSchema(QSqlDatabase &db);
    static int currentVersion(QSqlDatabase &db);
    static bool createDefaultData(QSqlDatabase &db);
};

} // namespace Iptv::Database
