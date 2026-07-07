#include "ffmpeg_utils.h"

namespace Iptv::Multimedia {

double FfmpegUtils::r2d(AVRational r)
{
    return r.num == 0 || r.den == 0 ? 0. :
               (double)r.num / (double)r.den;
}

int FfmpegUtils::inferSourceType(int width)
{
    //根据视频宽度推断源类型
    if (width >= 3840) return 4;  // 4K/8K
    if (width >= 1920) return 3;  // HD
    if (width >= 1280) return 2;  // SD
    if (width > 0) return 1;      // TS
    return 0;                     // 未知
}

QString FfmpegUtils::sourceTypeName(int type)
{
    switch (type) {
    case 0: return "Unknown";
    case 1: return "TS";
    case 2: return "SD";
    case 3: return "HD";
    case 4: return "4K";
    default: return "Unknown";
    }
}

} // namespace Iptv::Multimedia
