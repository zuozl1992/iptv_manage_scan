#pragma once

#include <QString>

extern "C" {
#include <libavutil/rational.h>
}

namespace Iptv::Multimedia {

namespace FfmpegUtils {
    double r2d(AVRational r);
    int inferSourceType(int width);
    QString sourceTypeName(int type);
}

} // namespace Iptv::Multimedia
