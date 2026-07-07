#pragma once

#include <QString>

namespace Iptv::Multimedia {

/**
 * @brief 流媒体信息结构体
 *        存储视频流探测结果
 */
struct StreamInfo {
    QString url;            ///< 探测的URL地址
    int width = 0;          ///< 视频宽度
    int height = 0;         ///< 视频高度
    int fps = 0;            ///< 帧率
    int sourceType = 0;     ///< 源类型（0=未知, 1=TS, 2=SD, 3=HD, 4=4K）
    bool success = false;   ///< 是否探测成功
};

} // namespace Iptv::Multimedia
