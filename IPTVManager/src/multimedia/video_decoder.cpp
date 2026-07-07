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
    
    //打开流媒体输入
    int ret = avformat_open_input(&m_formatCtx, url.toStdString().c_str(), nullptr, nullptr);
    if (ret != 0) {
        qWarning() << "Failed to open stream:" << url;
        return false;
    }
    
    //获取流信息
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
    
    //查找最佳视频流
    m_videoStreamIndex = av_find_best_stream(m_formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (m_videoStreamIndex < 0) {
        qWarning() << "No video stream found";
        return false;
    }
    
    //获取帧率
    m_streamInfo.fps = static_cast<int>(FfmpegUtils::r2d(
        m_formatCtx->streams[m_videoStreamIndex]->avg_frame_rate));
    
    //查找解码器
    const AVCodec *codec = avcodec_find_decoder(
        m_formatCtx->streams[m_videoStreamIndex]->codecpar->codec_id);
    if (!codec) {
        qWarning() << "No decoder found";
        return false;
    }
    
    //分配编解码器上下文
    m_codecCtx = avcodec_alloc_context3(codec);
    if (!m_codecCtx) {
        qWarning() << "Failed to allocate codec context";
        return false;
    }
    
    //复制编解码参数
    int ret = avcodec_parameters_to_context(m_codecCtx,
        m_formatCtx->streams[m_videoStreamIndex]->codecpar);
    if (ret < 0) {
        qWarning() << "Failed to copy codec parameters";
        return false;
    }
    
    //打开编解码器
    ret = avcodec_open2(m_codecCtx, codec, nullptr);
    if (ret != 0) {
        qWarning() << "Failed to open codec";
        return false;
    }
    
    //分配帧缓冲区
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
    
    //读取数据包并解码关键帧
    for (int attempt = 0; attempt < 10; ++attempt) {
        QList<AVPacket *> packets;
        int count = 500;
        int keyFrames = 0;
        int nonKeyAfterKey = 0;
        
        //读取数据包
        while (count-- > 0) {
            AVPacket *pkt = av_packet_alloc();
            if (!pkt) break;
            
            int ret = av_read_frame(m_formatCtx, pkt);
            if (ret != 0) {
                av_packet_free(&pkt);
                break;
            }
            
            //跳过非视频包
            if (pkt->stream_index != m_videoStreamIndex) {
                av_packet_free(&pkt);
                continue;
            }
            
            //首次尝试：跳过非关键帧
            if (packets.isEmpty() && attempt == 0) {
                if (!(pkt->flags & AV_PKT_FLAG_KEY)) {
                    av_packet_free(&pkt);
                    continue;
                }
            }
            
            //统计关键帧数量
            if (pkt->flags & AV_PKT_FLAG_KEY) {
                keyFrames++;
            } else {
                nonKeyAfterKey++;
            }
            
            //按DTS排序插入
            int i = 0;
            for (; i < packets.size(); ++i) {
                if (pkt->dts < packets.at(i)->dts) break;
            }
            packets.insert(i, pkt);
            
            if (packets.size() >= 100) break;
            if (nonKeyAfterKey >= 8 && keyFrames >= 1) break;
        }
        
        if (packets.isEmpty()) continue;
        
        //解码数据包，查找关键帧
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
                
                //跳过非关键帧和损坏帧
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
        
        //清理剩余数据包
        while (!packets.isEmpty()) {
            AVPacket *pkt = packets.first();
            packets.removeFirst();
            av_packet_free(&pkt);
        }
        
        if (decodedFrame) {
            //创建像素格式转换上下文（转为BGRA）
            SwsContext *swsCtx = sws_getContext(
                m_streamInfo.width, m_streamInfo.height,
                static_cast<AVPixelFormat>(decodedFrame->format),
                m_streamInfo.width, m_streamInfo.height,
                AV_PIX_FMT_BGRA, SWS_BICUBIC, nullptr, nullptr, nullptr);
            
            if (!swsCtx) {
                av_frame_free(&decodedFrame);
                return false;
            }
            
            //分配输出缓冲区
            freeFrameData();
            m_frameData = new uchar[m_streamInfo.width * m_streamInfo.height * 4];
            
            uint8_t *data[AV_NUM_DATA_POINTERS] = {0};
            data[0] = m_frameData;
            
            int linesize[AV_NUM_DATA_POINTERS] = {0};
            linesize[0] = m_streamInfo.width * 4;
            
            //执行像素格式转换
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
    //转移帧数据所有权
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
