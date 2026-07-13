#include "stream_probe.h"
#include "ffmpeg_utils.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libavutil/error.h>
}

#include <QDebug>

namespace Iptv::Multimedia {

StreamProbe::StreamProbe(QObject *parent)
    : QThread(parent)
{
}

void StreamProbe::setUrl(const QString &url)
{
    m_url = url;
}

void StreamProbe::setTimeout(int ms)
{
    m_timeout = ms;
}

void StreamProbe::setSlowScan(bool enabled)
{
    m_slowScan = enabled;
}

void StreamProbe::run()
{
    if (m_url.isEmpty()) {
        emit probeFailed(tr("URL is empty"));
        return;
    }

    char errorBuf[1024];
    AVFormatContext *ctx = nullptr;
    int ret;
    int retryCount = m_retryCount;
    int timeout = m_timeout;

L1:
    //打开流媒体输入（超时10000ms时不设置超时参数）
    if (timeout == 10000) {
        ret = avformat_open_input(&ctx, m_url.toStdString().c_str(), nullptr, nullptr);
    } else {
        AVDictionary *options = nullptr;
        av_dict_set(&options, "timeout", QString("%1").arg(timeout * 1000).toStdString().c_str(), 0);
        ret = avformat_open_input(&ctx, m_url.toStdString().c_str(), nullptr, &options);
    }

    if (ret != 0) {
        av_strerror(ret, errorBuf, sizeof(errorBuf));
        qDebug() << "Failed to open" << m_url << "error:" << errorBuf;
        avformat_close_input(&ctx);

        //EIO错误且启用慢速扫描时，减半超时重试
        if (retryCount-- > 0 && ret == -5 && m_slowScan) {
            timeout /= 2;
            goto L1;
        }

        emit probeFailed(QString::fromUtf8(errorBuf));
        return;
    }

    //获取流信息
    ret = avformat_find_stream_info(ctx, nullptr);
    if (ret < 0) {
        av_strerror(ret, errorBuf, sizeof(errorBuf));
        qDebug() << "Failed to find stream info" << m_url << "error:" << errorBuf;
        avformat_close_input(&ctx);
        emit probeFailed(QString::fromUtf8(errorBuf));
        return;
    }

    //查找最佳视频流
    int vIndex = av_find_best_stream(ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (vIndex < 0) {
        av_strerror(vIndex, errorBuf, sizeof(errorBuf));
        qDebug() << "No video stream found" << m_url << "error:" << errorBuf;
        avformat_close_input(&ctx);
        emit probeFailed(QString::fromUtf8(errorBuf));
        return;
    }

    //提取流信息
    StreamInfo info;
    info.url = m_url;
    info.width = ctx->streams[vIndex]->codecpar->width;
    info.height = ctx->streams[vIndex]->codecpar->height;
    info.fps = static_cast<int>(FfmpegUtils::r2d(ctx->streams[vIndex]->avg_frame_rate));
    info.sourceType = FfmpegUtils::inferSourceType(info.width);
    info.success = true;

    avformat_close_input(&ctx);

    emit probeSucceeded(info);
}

} // namespace Iptv::Multimedia
