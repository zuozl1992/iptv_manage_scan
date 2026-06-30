#pragma once

#include <QString>
#include <QVariant>

namespace Iptv::Core {

class Bootstrap
{
public:
    Bootstrap();

    QString configPath() const;
    QString dbPath() const;
    
    void setConfigPath(const QString &path);
    void setDbPath(const QString &path);
    
    bool isValid() const;
    bool load();
    bool save() const;

private:
    QString m_configPath;
    QString m_dbPath;
    QString m_bootstrapPath;
};

} // namespace Iptv::Core
