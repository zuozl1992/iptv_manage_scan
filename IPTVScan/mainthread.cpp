#include "mainthread.h"
#include "scanthread.h"
#include <QReadWriteLock>
#include <QDebug>

MainThread::MainThread(QObject *parent)
    : QThread{parent}
    , posLock(new QReadWriteLock)
    , countLock(new QReadWriteLock)
{}

void MainThread::stFinishedSlot()
{
    ScanThread *st = (ScanThread *)sender();
    bool check = st->check;
    QString ip = getIpFromUrl(st->url);
    if(check){
        ipCheck.insert(ip, true);
        emit checkOk(st->url, st->width, st->height, st->fps);
    }
    delete st;
    countLock->lockForWrite();
    count--;
    countLock->unlock();
    int t,c,e;
    posLock->lockForWrite();
    pos++;
    t = pos;
    if(check)
        okCount++;
    else
        errorCount++;
    c = okCount;
    e = errorCount;
    posLock->unlock();
    emit progressChange(t, urlList.length(), c, e);
}

void MainThread::run()
{
    for(int i = 0; i < urlList.length(); i++){
        while(1){
            if(exit)
                return;
            bool ok = false;
            countLock->lockForRead();
            ok = count >= thMax;
            countLock->unlock();
            if(ok)
                msleep(10);
            else
                break;
        }
        if(exit)
            return;
        QString url = urlList.at(i);
        emit startCheckTh(url);
        if(autoStep){
            QString ip = getIpFromUrl(url);
            bool step = ipCheck.value(ip, false);
            if(step){
                int c,e,t;
                posLock->lockForWrite();
                pos++;
                errorCount++;
                t = pos;
                c = okCount;
                e = errorCount;
                posLock->unlock();
                emit progressChange(t, urlList.length(), c, e);
                continue;
            }
        }
        ScanThread *st = new ScanThread;
        st->url = url;
        st->timeout = timeout;
        st->slowThread = slowThread;
        connect(st, &ScanThread::finished,
                this, &MainThread::stFinishedSlot);
        st->start();
        countLock->lockForWrite();
        count++;
        countLock->unlock();
        msleep(1000);
    }
}

QString MainThread::getIpFromUrl(QString url)
{
    int i1 = url.indexOf("udp");
    int i2 = url.indexOf("rtp");
    int start, end;
    if(i1 < 0 && i2 < 0){
        start = url.indexOf("://") + 3;
        end = url.indexOf(":", start);
    }
    else{
        start = (i1 < 0 ? i2 : i1) + 4;
        end = url.indexOf(":", start);
    }
    return url.mid(start, end-start);
}
