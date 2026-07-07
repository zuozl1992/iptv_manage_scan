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
    
    //带重试的流打开（用于FPS检测）
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
        
        //查找视频流
        if (!decoder.findVideoStream()) {
            emit probeFailed(tr("没有获取到视频流"));
            return;
        }
        
        emit progressChanged(20);
        
        StreamInfo info = decoder.streamInfo();
        //FPS为0时重试
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
    
    //解码关键帧
    if (!decoder.decodeKeyFrame()) {
        emit probeFailed(tr("解码失败"));
        return;
    }
    
    emit progressChanged(90);
    
    //获取解码结果
    StreamInfo info = decoder.streamInfo();
    uchar *frameData = decoder.takeFrameData();
    
    //根据视频宽度推断源类型
    int oldType = m_sourceInfo.value("type").toInt();
    int type = FfmpegUtils::inferSourceType(info.width);
    if (oldType == 0) {
        type = 0;  //保持测试/未知类型
    }
    
    info.sourceType = type;
    info.frameData = frameData;
    
    emit progressChanged(100);
    emit probeSucceeded(info, m_sourceInfo);
}

} // namespace Iptv::Multimedia
