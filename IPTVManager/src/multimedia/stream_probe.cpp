#include "stream_probe.h"
#include "video_decoder.h"
#include "ffmpeg_utils.h"

extern "C" {
#include <libavformat/avformat.h>
}

#include <QDebug>

namespace Iptv::Multimedia {

StreamProbe::StreamProbe(QObject *parent)
    : QThread(parent)
{
}

void StreamProbe::probe(const QString &url, const QJsonObject &sourceInfo)
{
    if (isRunning()) {
        qWarning() << "Stream probe already running";
        return;
    }
    
    m_url = url;
    m_sourceInfo = sourceInfo;
    start();
}

void StreamProbe::run()
{
    emit progressChanged(1);
    
    VideoDecoder decoder;
    
    // Open stream with retry for FPS detection
    int retryCount = 0;
    bool opened = false;
    qDebug() << "111111" << m_url;
    while (retryCount < 3) {
        if (!decoder.open(m_url)) {
            if (retryCount == 0) {
                emit probeFailed(tr("打开失败"));
                return;
            }
            break;
        }
        
        if (!decoder.findVideoStream()) {
            emit probeFailed(tr("没有获取到视频流"));
            return;
        }
        
        emit progressChanged(20);
        
        StreamInfo info = decoder.streamInfo();
        if (info.fps <= 0) {
            decoder.close();
            retryCount++;
            continue;
        }
        
        opened = true;
        break;
    }
    
    if (!opened) {
        emit probeFailed(tr("媒体信息获取失败"));
        return;
    }
    
    emit progressChanged(40);
    
    // Decode keyframe
    if (!decoder.decodeKeyFrame()) {
        emit probeFailed(tr("解码失败"));
        return;
    }
    
    emit progressChanged(90);
    
    // Get results
    StreamInfo info = decoder.streamInfo();
    uchar *frameData = decoder.takeFrameData();
    
    // Infer source type from width
    int oldType = m_sourceInfo.value("type").toInt();
    int type = FfmpegUtils::inferSourceType(info.width);
    if (oldType == 0) {
        type = 0;  // Keep as test/unknown
    }
    
    info.sourceType = type;
    info.frameData = frameData;
    
    emit progressChanged(100);
    emit probeSucceeded(info, m_sourceInfo);
}

} // namespace Iptv::Multimedia
