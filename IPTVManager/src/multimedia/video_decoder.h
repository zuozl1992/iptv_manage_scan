#pragma once

#include "stream_info.h"
#include <QString>

struct AVFormatContext;
struct AVCodecContext;
struct AVFrame;
struct SwsContext;

namespace Iptv::Multimedia {

class VideoDecoder
{
public:
    VideoDecoder();
    ~VideoDecoder();

    bool open(const QString &url);
    bool findVideoStream();
    bool decodeKeyFrame();
    
    StreamInfo streamInfo() const;
    uchar *takeFrameData();
    
    void close();
    bool isOpen() const;

private:
    void freeFrameData();
    
    AVFormatContext *m_formatCtx = nullptr;
    AVCodecContext *m_codecCtx = nullptr;
    AVFrame *m_frame = nullptr;
    SwsContext *m_swsCtx = nullptr;
    
    int m_videoStreamIndex = -1;
    StreamInfo m_streamInfo;
    uchar *m_frameData = nullptr;
};

} // namespace Iptv::Multimedia
