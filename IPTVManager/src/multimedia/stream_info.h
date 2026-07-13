#pragma once

#include <QString>

namespace Iptv::Multimedia {

/**
 * @brief 流媒体信息结构体
 *        存储视频流的解码信息
 */
struct StreamInfo {
    int width = 0;              ///< 视频宽度
    int height = 0;             ///< 视频高度
    int fps = 0;                ///< 帧率
    int sourceType = 0;         ///< 源类型（0=TS, 1=SD, 2=HD, 3=4K, 4=8K）
    uchar *frameData = nullptr; ///< BGRA像素数据（调用方负责释放）
    QString codecName;          ///< 编解码器名称
};

} // namespace Iptv::Multimedia
