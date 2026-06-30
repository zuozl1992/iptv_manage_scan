#ifndef MAINTHREAD_H
#define MAINTHREAD_H

#include <QThread>
#include <QHash>
class QReadWriteLock;
class MainThread : public QThread
{
    Q_OBJECT
    friend class MainPage;
public:
    explicit MainThread(QObject *parent = nullptr);

signals:
    void checkOk(QString url, int width, int height, int fps);
    void progressChange(int pos, int count, int okCount, int errorCount);
    void startCheckTh(QString url);

protected slots:
    void stFinishedSlot();

protected:
    void run();
    QString getIpFromUrl(QString url);
    int count = 0;
    int pos = 0;
    bool exit = false;
    QReadWriteLock *posLock, *countLock;
    QString mainUrl;
    int thMax = 4;
    int timeout = -1;
    int okCount = 0;
    int errorCount = 0;
    QStringList urlList;
    QHash<QString, bool> ipCheck;
    bool autoStep = false;
    bool slowThread = false;
};

#endif // MAINTHREAD_H
