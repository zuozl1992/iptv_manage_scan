#pragma once

#include <QString>

namespace Iptv::Multimedia {

struct StreamInfo {
    QString url;           // 探测的URL地址
    int width = 0;
    int height = 0;
    int fps = 0;
    int sourceType = 0;    // 0=未知, 1=TS, 2=SD, 3=HD, 4=4K
    bool success = false;
};

} // namespace Iptv::Multimedia
