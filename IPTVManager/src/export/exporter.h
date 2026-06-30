#pragma once

#include <QString>
#include <QStringList>

namespace Iptv::Export {

struct ExportOptions {
    QString urlPrefix;
    QString urlType;  // "udp" or "rtp"
    QString fccUrl;
    bool fccEnabled = false;
    bool addLogo = false;
    bool addHdSuffix = false;
    bool mergeChannels = false;
    QString logoBaseUrl;
    QStringList groups;
};

} // namespace Iptv::Export
