#ifndef SCANTHREAD_H
#define SCANTHREAD_H

#include <QThread>

class ScanThread : public QThread
{
    Q_OBJECT
    friend class MainThread;
public:
    explicit ScanThread(QObject *parent = nullptr);

protected:
    void run();
    bool checkVideo(QString url);
    QString url;
    int timeout = -1;
    bool check = false;
    int width, height, fps;
    int firstFlag = 2;
    bool slowThread = false;
};

#endif // SCANTHREAD_H
