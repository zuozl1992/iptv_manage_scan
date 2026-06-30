#pragma once

#include <QString>

namespace Iptv::Multimedia {

struct StreamInfo {
    int width = 0;
    int height = 0;
    int fps = 0;
    int sourceType = 0;  // 0=TS, 1=SD, 2=HD, 3=4K, 4=8K
    uchar *frameData = nullptr;  // BGRA pixel data, caller owns
    QString codecName;
};

} // namespace Iptv::Multimedia
