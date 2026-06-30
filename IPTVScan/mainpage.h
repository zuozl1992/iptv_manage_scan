#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainPage;
}
QT_END_NAMESPACE
class MainThread;
class MainPage : public QWidget
{
    Q_OBJECT

public:
    MainPage(QString SETFILENAME, QWidget *parent = nullptr);
    ~MainPage();

private slots:
    void checkOkSlot(QString url, int width, int height, int fps);
    void progressChangeSlot(int pos, int count, int okCount, int errorCount);
    void startCheckThSlot(QString url);
    void on_btnStart_clicked();
    void on_btnSave_clicked();
    void on_btnStop_clicked();
    void on_btnClear_clicked();
    void on_btnRec_clicked();

    void on_btnOpenFile_clicked();

private:
    QStringList createList(QString text);
    bool createUrl(QString url, QStringList &re);
    bool createUrlFromFile(QString path, QStringList &re);
    void loadConfig();
    void saveConfig();
    void closeEvent(QCloseEvent *e);
    MainThread *mt;
    Ui::MainPage *ui;
    QStringList sl;
    int runIndex;
    QString SETFILENAME;
};
#endif // MAINPAGE_H
