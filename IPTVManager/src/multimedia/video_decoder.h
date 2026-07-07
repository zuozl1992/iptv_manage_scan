#pragma once

#include "stream_info.h"
#include <QString>

struct AVFormatContext;
struct AVCodecContext;
struct AVFrame;
struct SwsContext;

namespace Iptv::Multimedia {

/**
 * @brief FFmpeg视频解码器
 *        封装FFmpeg的视频流打开、解码和帧数据提取操作
 */
class VideoDecoder
{
public:
    VideoDecoder();
    ~VideoDecoder();

    /** @brief 打开流媒体URL
     *  @param url 流媒体地址
     *  @return 是否打开成功 */
    bool open(const QString &url);

    /** @brief 查找视频流 */
    bool findVideoStream();

    /** @brief 解码一个关键帧 */
    bool decodeKeyFrame();

    /** @brief 获取流媒体信息 */
    StreamInfo streamInfo() const;

    /** @brief 取出帧数据所有权（调用方负责释放） */
    uchar *takeFrameData();

    /** @brief 关闭解码器并释放资源 */
    void close();

    /** @brief 检查解码器是否已打开 */
    bool isOpen() const;

private:
    void freeFrameData();

    AVFormatContext *m_formatCtx = nullptr;  ///< 格式上下文
    AVCodecContext *m_codecCtx = nullptr;    ///< 编解码器上下文
    AVFrame *m_frame = nullptr;             ///< 解码帧
    SwsContext *m_swsCtx = nullptr;         ///< 像素格式转换上下文

    int m_videoStreamIndex = -1;    ///< 视频流索引
    StreamInfo m_streamInfo;        ///< 流媒体信息
    uchar *m_frameData = nullptr;   ///< BGRA帧数据
};

} // namespace Iptv::Multimedia
