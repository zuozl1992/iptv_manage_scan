#pragma once

#include <QString>
#include <QDateTime>

namespace Iptv::Database {

struct TvChannel {
    int id = -1;
    int channelId = 0;
    QString name;
    QString group;
    QString city;
    QString describe;
    QString notes;
    QString logoName;
};

struct TvSource {
    int id = -1;
    int tvId = -1;
    QString ip;
    int port = 0;
    int width = 0;
    int height = 0;
    int fps = 0;
    QDateTime lastCheckDate;
    int type = 0;  // source_type.id
    QString notes;
};

struct SourceType {
    int id = 0;
    QString name;  // "TS", "SD", "HD", "4K", "8K"
};

} // namespace Iptv::Database
