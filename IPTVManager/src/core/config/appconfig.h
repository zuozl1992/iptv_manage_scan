#pragma once

#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringList>

namespace Iptv::Core {

class AppConfig : public QObject
{
    Q_OBJECT

public:
    static AppConfig *instance();

    void init(const QString &configPath);

    // Generic accessors
    QVariant value(const QString &key, const QVariant &defaultValue = {}) const;
    void setValue(const QString &key, const QVariant &value);

    // Typed accessors
    QString httpUrl() const;
    void setHttpUrl(const QString &url);

    QString fileUrl() const;
    void setFileUrl(const QString &url);

    QString groupAddress() const;
    void setGroupAddress(const QString &addr);

    QString groupPort() const;
    void setGroupPort(const QString &port);

    int urlTypeIndex() const;
    void setUrlTypeIndex(int index);

    bool addTestEnabled() const;
    void setAddTestEnabled(bool enabled);

    bool outByOne() const;
    void setOutByOne(bool enabled);

    bool mergeChannels() const;
    void setMergeChannels(bool enabled);

    bool addLogoEnabled() const;
    void setAddLogoEnabled(bool enabled);

    bool addHdSuffix() const;
    void setAddHdSuffix(bool enabled);

    QString fccUrl() const;
    void setFccUrl(const QString &url);

    bool fccEnabled() const;
    void setFccEnabled(bool enabled);

    QStringList selectedGroups() const;
    void setSelectedGroups(const QStringList &groups);

private:
    explicit AppConfig(QObject *parent = nullptr);

    QSettings *m_settings = nullptr;
    static AppConfig *s_instance;
};

} // namespace Iptv::Core
