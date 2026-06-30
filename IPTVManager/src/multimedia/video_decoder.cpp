#include "video_decoder.h"
#include "ffmpeg_utils.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavcodec/packet.h>
#include <libavutil/frame.h>
#include <libswscale/swscale.h>
}

#include <QDebug>

namespace Iptv::Multimedia {

VideoDecoder::VideoDecoder()
{
}

VideoDecoder::~VideoDecoder()
{
    close();
}

bool VideoDecoder::open(const QString &url)
{
    close();
    
    int ret = avformat_open_input(&m_formatCtx, url.toStdString().c_str(), nullptr, nullptr);
    if (ret != 0) {
        qWarning() << "Failed to open stream:" << url;
        return false;
    }
    
    ret = avformat_find_stream_info(m_formatCtx, nullptr);
    if (ret < 0) {
        qWarning() << "Failed to find stream info";
        close();
        return false;
    }
    
    return true;
}

bool VideoDecoder::findVideoStream()
{
    if (!m_formatCtx) return false;
    
    m_videoStreamIndex = av_find_best_stream(m_formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (m_videoStreamIndex < 0) {
        qWarning() << "No video stream found";
        return false;
    }
    
    // Get FPS
    m_streamInfo.fps = static_cast<int>(FfmpegUtils::r2d(
        m_formatCtx->streams[m_videoStreamIndex]->avg_frame_rate));
    
    // Find decoder
    const AVCodec *codec = avcodec_find_decoder(
        m_formatCtx->streams[m_videoStreamIndex]->codecpar->codec_id);
    if (!codec) {
        qWarning() << "No decoder found";
        return false;
    }
    
    // Allocate codec context
    m_codecCtx = avcodec_alloc_context3(codec);
    if (!m_codecCtx) {
        qWarning() << "Failed to allocate codec context";
        return false;
    }
    
    // Copy parameters
    int ret = avcodec_parameters_to_context(m_codecCtx,
        m_formatCtx->streams[m_videoStreamIndex]->codecpar);
    if (ret < 0) {
        qWarning() << "Failed to copy codec parameters";
        return false;
    }
    
    // Open codec
    ret = avcodec_open2(m_codecCtx, codec, nullptr);
    if (ret != 0) {
        qWarning() << "Failed to open codec";
        return false;
    }
    
    // Allocate frame
    m_frame = av_frame_alloc();
    if (!m_frame) {
        qWarning() << "Failed to allocate frame";
        return false;
    }
    
    m_streamInfo.width = m_codecCtx->width;
    m_streamInfo.height = m_codecCtx->height;
    m_streamInfo.codecName = QString::fromUtf8(codec->name);
    
    return true;
}

bool VideoDecoder::decodeKeyFrame()
{
    if (!m_formatCtx || !m_codecCtx || !m_frame) return false;
    
    // Read packets and decode keyframe
    for (int attempt = 0; attempt < 10; ++attempt) {
        QList<AVPacket *> packets;
        int count = 500;
        int keyFrames = 0;
        int nonKeyAfterKey = 0;
        
        // Read packets
        while (count-- > 0) {
            AVPacket *pkt = av_packet_alloc();
            if (!pkt) break;
            
            int ret = av_read_frame(m_formatCtx, pkt);
            if (ret != 0) {
                av_packet_free(&pkt);
                break;
            }
            
            // Skip non-video packets
            if (pkt->stream_index != m_videoStreamIndex) {
                av_packet_free(&pkt);
                continue;
            }
            
            // First pass: skip until keyframe
            if (packets.isEmpty() && attempt == 0) {
                if (!(pkt->flags & AV_PKT_FLAG_KEY)) {
                    av_packet_free(&pkt);
                    continue;
                }
            }
            
            // Count keyframes
            if (pkt->flags & AV_PKT_FLAG_KEY) {
                keyFrames++;
            } else {
                nonKeyAfterKey++;
            }
            
            // Insert sorted by DTS
            int i = 0;
            for (; i < packets.size(); ++i) {
                if (pkt->dts < packets.at(i)->dts) break;
            }
            packets.insert(i, pkt);
            
            if (packets.size() >= 100) break;
            if (nonKeyAfterKey >= 8 && keyFrames >= 1) break;
        }
        
        if (packets.isEmpty()) continue;
        
        // Decode packets, looking for keyframe
        AVFrame *decodedFrame = nullptr;
        while (!packets.isEmpty()) {
            AVPacket *pkt = packets.first();
            packets.removeFirst();
            
            int ret = avcodec_send_packet(m_codecCtx, pkt);
            av_packet_free(&pkt);
            
            if (ret != 0) break;
            
            while (true) {
                decodedFrame = av_frame_alloc();
                ret = avcodec_receive_frame(m_codecCtx, decodedFrame);
                if (ret != 0) {
                    av_frame_free(&decodedFrame);
                    decodedFrame = nullptr;
                    break;
                }
                
                // Skip non-keyframes and corrupt frames
                if (!(decodedFrame->flags & AV_FRAME_FLAG_KEY) ||
                    (decodedFrame->flags & AV_FRAME_FLAG_CORRUPT)) {
                    av_frame_free(&decodedFrame);
                    decodedFrame = nullptr;
                    continue;
                }
                
                break;
            }
            
            if (decodedFrame) break;
        }
        
        // Clean up remaining packets
        while (!packets.isEmpty()) {
            AVPacket *pkt = packets.first();
            packets.removeFirst();
            av_packet_free(&pkt);
        }
        
        if (decodedFrame) {
            // Convert to BGRA
            SwsContext *swsCtx = sws_getContext(
                m_streamInfo.width, m_streamInfo.height,
                static_cast<AVPixelFormat>(decodedFrame->format),
                m_streamInfo.width, m_streamInfo.height,
                AV_PIX_FMT_BGRA, SWS_BICUBIC, nullptr, nullptr, nullptr);
            
            if (!swsCtx) {
                av_frame_free(&decodedFrame);
                return false;
            }
            
            // Allocate output buffer
            freeFrameData();
            m_frameData = new uchar[m_streamInfo.width * m_streamInfo.height * 4];
            
            uint8_t *data[AV_NUM_DATA_POINTERS] = {0};
            data[0] = m_frameData;
            
            int linesize[AV_NUM_DATA_POINTERS] = {0};
            linesize[0] = m_streamInfo.width * 4;
            
            int h = sws_scale(swsCtx, decodedFrame->data, decodedFrame->linesize,
                              0, decodedFrame->height, data, linesize);
            
            av_frame_free(&decodedFrame);
            sws_freeContext(swsCtx);
            
            if (h > 0) {
                return true;
            }
        }
    }
    
    return false;
}

StreamInfo VideoDecoder::streamInfo() const
{
    return m_streamInfo;
}

uchar *VideoDecoder::takeFrameData()
{
    uchar *data = m_frameData;
    m_frameData = nullptr;
    return data;
}

void VideoDecoder::close()
{
    freeFrameData();
    
    if (m_frame) {
        av_frame_free(&m_frame);
        m_frame = nullptr;
    }
    
    if (m_codecCtx) {
        avcodec_free_context(&m_codecCtx);
        m_codecCtx = nullptr;
    }
    
    if (m_formatCtx) {
        avformat_close_input(&m_formatCtx);
        m_formatCtx = nullptr;
    }
    
    m_videoStreamIndex = -1;
    m_streamInfo = StreamInfo();
}

bool VideoDecoder::isOpen() const
{
    return m_formatCtx != nullptr;
}

void VideoDecoder::freeFrameData()
{
    if (m_frameData) {
        delete[] m_frameData;
        m_frameData = nullptr;
    }
}

} // namespace Iptv::Multimedia
