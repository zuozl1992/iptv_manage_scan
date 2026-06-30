#include "scanthread.h"
extern "C"{
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libavutil/error.h>
}
#include <QDebug>
#include <QCoreApplication>
#include <QProcess>
static double r2d(AVRational r)
{
    return r.num == 0 || r.den == 0 ? 0. :
               (double)r.num / (double)r.den;
}

ScanThread::ScanThread(QObject *parent)
    : QThread{parent}
{}

// qint64 ScanThread::GetVideo_TotalTime(QString video_name)
// {
//     QString cmd=QString("%1 -i \"%2\"").arg("ffprobe.exe").arg(video_name);
//     qDebug() << cmd;
//     QStringList ls;
//     ls << "-i" << video_name;;
//     QProcess process;
//     process.setProcessChannelMode(QProcess::MergedChannels);
//     process.start("ffprobe.exe", ls);
//     process.waitForFinished();
//     process.waitForReadyRead();
//     QString qba  = process.readAll();
//     // qDebug() << qba;
//     char *find_p=strstr(qba.toUtf8().data()," Video:");
//     if(find_p){
//         find_p += 6;
//         QString find_str=find_p;
//         qDebug() << find_str;
//     }
//     // if(find_p)
//     // {
//     //     find_p+=11;
//     //     QString find_str=find_p;
//     //     find_str=find_str.section(',', 0, 0);

//     //     qDebug()<<"视频总时间:"<<find_str;

//     //     QTime time=QTime::fromString(find_str);
//     //     //qDebug()<<"时:"<<time.hour();
//     //     //qDebug()<<"分:"<<time.minute();
//     //     //qDebug()<<"秒:"<<time.second();
//     //     //qDebug()<<"毫秒:"<<time.msec();

//     //     //视频总时间
//     //     video_time_lent=time.hour()*60*60*1000+time.minute()*60*1000+time.second()*1000+time.msec();
//     // }
//     return 0;
// }

// #define DEBUG
void ScanThread::run()
{

    char eb[1024];
    if(url.isEmpty()){
        check = false;
        return;
    }

    AVFormatContext *ctx = nullptr;
    int re;

L1:;
    if(timeout == 10000){
//         if(!checkVideo(url)){
//             check = false;
//             return;
//         }
        re = avformat_open_input(&ctx, url.toStdString().c_str(), nullptr, nullptr);
    }
    else{
        AVDictionary *options = nullptr;
        av_dict_set(&options, "timeout", QString("%1").arg(timeout*1000).toStdString().c_str(), 0);
        re = avformat_open_input(&ctx, url.toStdString().c_str(), nullptr, &options);
    }
    if(re != 0){
        av_strerror(re, eb, sizeof(eb));
#ifdef DEBUG
        qDebug() << 34 << "open " << url << " res=" << re << " err=" << eb;
#endif
        avformat_close_input(&ctx);
        if(firstFlag-- >= 0 && re == -5 && slowThread){
            timeout /= 2;
            goto L1;
        }

        check = false;
        return;
    }
    re = avformat_find_stream_info(ctx, nullptr);
    if(re < 0)
    {
        av_strerror(re, eb, sizeof(eb));
#ifdef DEBUG
        qDebug() << 43 << "find stream info " << url << " res=" << re << " err=" << eb;
#endif
        avformat_close_input(&ctx);
        check = false;
        return;
    }
    int vIndex = av_find_best_stream(ctx, AVMEDIA_TYPE_VIDEO,
                                 -1, -1, nullptr, 0);
    if(vIndex < 0)
    {
        av_strerror(re, eb, sizeof(eb));
#ifdef DEBUG
        qDebug() << 64 << "find video stream " << url << " res=" << re << " err=" << eb;
#endif
        avformat_close_input(&ctx);
        check = false;
        return;
    }
    width = ctx->streams[vIndex]->codecpar->width;
    height = ctx->streams[vIndex]->codecpar->height;
    fps = r2d(ctx->streams[vIndex]->avg_frame_rate);
#ifdef DEBUG
    qDebug() << 66 << "get info " << url << " w=" << width << " h=" << height << " fps=" << fps;
#endif
    avformat_close_input(&ctx);
    // if(width > 0 && height >0 && fps > 0)
    //     check = true;
    // else
    //     goto L1;
    check = true;
}

bool ScanThread::checkVideo(QString url)
{
    QStringList ls;
    ls << "-i" << url;;
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start("ffprobe.exe", ls);
    process.waitForFinished();
    process.waitForReadyRead();
    QString qba  = process.readAll();
    char *find_p=strstr(qba.toUtf8().data()," Video:");
    if(find_p){
        find_p += 8;
        QString find_str=find_p;
        if(!find_str.isEmpty())
            return true;
    }
    return false;
}
