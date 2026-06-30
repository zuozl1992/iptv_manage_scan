#include "mainpage.h"
#include "ui_mainpage.h"
#include "mainthread.h"
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSettings>
#include <QCloseEvent>

// #define SETFILENAME "scan_config.ini"
#define DEFGROUP    "set"
#define URL         "url"
#define THMAX       "thread_max"
#define TIMEOUT     "time_out"
#define AUTOSTEP    "auto_step"
#define FILEPATH    "modelfile_path"
#define USEURL      "use_url"
#define ADDTS       "add_ts"
#define SLOWSCAN    "slow_scan"

MainPage::MainPage(QString SETFILENAME, QWidget *parent)
    : QWidget(parent)
    , SETFILENAME(SETFILENAME)
    , ui(new Ui::MainPage)
    , mt(new MainThread)
{
    ui->setupUi(this);
    setWindowTitle(tr("IPTV扫描"));
    resize(800, 650);
    loadConfig();
    connect(mt, &MainThread::checkOk,
            this, &MainPage::checkOkSlot);
    connect(mt, &MainThread::progressChange,
            this, &MainPage::progressChangeSlot);
    connect(mt, &MainThread::startCheckTh,
            this, &MainPage::startCheckThSlot);
    connect(mt, &MainThread::finished,
            this, [&](){ui->btnStart->setEnabled(true);});
}

MainPage::~MainPage()
{
    saveConfig();
    delete ui;
}

void MainPage::checkOkSlot(QString url, int width, int height, int fps)
{
    QString log;
    if(ui->cbAddTS->isChecked())
        log = QString("待整理 TS %1x%2(%3),%4").arg(width).arg(height).arg(fps).arg(url);
    else
        log = QString("%1x%2(%3),%4").arg(width).arg(height).arg(fps).arg(url);
    sl.append(log);
    ui->tbLog->append(log);
}

void MainPage::progressChangeSlot(int pos, int count, int okCount, int errorCount)
{
    if(pos < runIndex)
        return;
    this->runIndex = pos;
    ui->lbProgress->setText(QString("%1/%2").arg(pos).arg(count));
    ui->progressBar->setValue(pos * 100 / count);
    ui->lbStatus2->setText(tr("成功：%1，失败：%2").arg(okCount).arg(errorCount));
}

void MainPage::startCheckThSlot(QString url)
{
    ui->lbStatus->setText(url);
}

void MainPage::on_btnStart_clicked()
{
    QMessageBox::StandardButton r = QMessageBox::question(this, tr("提示"), tr("确认开始？开始后将清空上次结果。"));
    if(r != QMessageBox::Yes)
        return;
    QStringList re;
    bool ok = false;
    if(ui->rbUseUrl->isChecked())
        ok = createUrl(ui->leURL->text(), re);
    else{
        ok = createUrlFromFile(ui->lbFilePath->text(), re);
    }
    if(!ok){
        QMessageBox::warning(
            this, tr("警告"), tr("输入有误"));
        return;
    }
    ui->tbLog->clear();
    mt->urlList = re;
    mt->timeout = ui->sbTimeout->value();
    mt->mainUrl = ui->leURL->text();
    mt->thMax = ui->sbThMax->value();
    mt->autoStep = ui->cbAutoStep->isChecked();
    mt->exit = false;
    mt->count = 0;
    mt->pos = 0;
    mt->okCount = 0;
    mt->errorCount = 0;
    mt->ipCheck.clear();
    mt->slowThread = ui->cbSlotScan->isChecked();
    this->runIndex = 0;
    ui->lbStatus->setText("");
    ui->lbStatus2->setText("--");
    ui->progressBar->setValue(0);
    ui->lbProgress->setText(QString("0/%1").arg(re.length()));
    ui->btnStart->setEnabled(false);
    mt->start();
}

void MainPage::on_btnSave_clicked()
{
    QString path = QFileDialog::getSaveFileName(
        this, tr("保存文件"), "", "txt(*.txt)");
    if(path.isEmpty())
        return;
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;
    file.write(ui->tbLog->toPlainText().toLocal8Bit());
    file.close();
}


void MainPage::on_btnStop_clicked()
{
    mt->exit = true;
    mt->quit();
}

void MainPage::on_btnClear_clicked()
{
    ui->tbLog->clear();
}

QStringList MainPage::createList(QString text)
{
    QStringList re;
    QStringList temp = text.split("#");
    for(int i = 0; i < temp.length(); i++){
        if(temp.at(i).startsWith("[")){
            QString r = temp.at(i).mid(1, temp.at(i).length()-2);
            QStringList rl = r.remove(QRegularExpression("\\s")).split("-");
            if(rl.length() != 2)
                continue;
            int s = rl.at(0).toInt();
            int e = rl.at(1).toInt();
            int len = rl.at(0).length();
            if(s >= e || s <= 0 || e <= 0)
                continue;
            for(int j = s; j <= e; j++)
                re.append(QString("%1").arg(j, len, 10, QChar('0')));
        }
        else
            re.append(temp.at(i));
    }
    return re;
}

bool MainPage::createUrl(QString url, QStringList &re)
{
    QString temp = url;
    if(temp.split("{").length() > 4)
        return false;
    int sIndex = temp.indexOf("{");
    int eIndex = temp.indexOf("}");
    if(sIndex < 0 && eIndex < 0){
        re.append(temp);
        return true;
    }
    else if(sIndex < 0 || eIndex < 0 || eIndex <= sIndex){
        return false;
    }
    QStringList strList = createList(temp.mid(sIndex+1, eIndex-sIndex-1));
    QString a = temp.mid(0, sIndex);
    QString b = temp.mid(eIndex+1);
    if(b.indexOf("{") >= 0){
        QStringList tre;
        bool ok = createUrl(b, tre);
        if(!ok)
            return false;
        for(int i = 0; i < strList.length(); i++){
            for(int j = 0; j < tre.length(); j++){
                re.append(QString("%1%2%3")
                              .arg(a)
                              .arg(strList.at(i))
                              .arg(tre.at(j)));
            }
        }
    }
    else{
        for(int i = 0; i < strList.length(); i++){
            re.append(QString("%1%2%3")
                               .arg(a)
                               .arg(strList.at(i))
                               .arg(b));
        }
    }
    return true;
}

bool MainPage::createUrlFromFile(QString path, QStringList &re)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)){
        return false;
    }
    while(true){
        QByteArray data = file.readLine();
        if(data.isEmpty())
            break;
        if(data.lastIndexOf('\n') >= 0)
            data.removeLast();
        if(data.lastIndexOf('\r') >= 0)
            data.removeLast();
        bool ok = createUrl(QString::fromLocal8Bit(data), re);
        if(!ok)
            return false;
    }
    return true;
}

void MainPage::loadConfig()
{
    QSettings set(SETFILENAME, QSettings::IniFormat);
    set.beginGroup(DEFGROUP);
    QStringList keys = set.allKeys();
    if(!keys.contains(URL))
        set.setValue(URL, ui->leURL->text());
    else
        ui->leURL->setText(set.value(URL).toString());
    if(!keys.contains(THMAX))
        set.setValue(THMAX, ui->sbThMax->value());
    else
        ui->sbThMax->setValue(set.value(THMAX).toInt());
    if(!keys.contains(TIMEOUT))
        set.setValue(TIMEOUT, ui->sbTimeout->value());
    else
        ui->sbTimeout->setValue(set.value(TIMEOUT).toInt());
    if(!keys.contains(AUTOSTEP))
        set.setValue(AUTOSTEP, ui->cbAutoStep->isChecked());
    else
        ui->cbAutoStep->setChecked(set.value(AUTOSTEP).toBool());
    if(!keys.contains(USEURL))
        set.setValue(USEURL, ui->rbUseUrl->isChecked());
    else
        set.value(USEURL).toBool() ? ui->rbUseUrl->setChecked(true) : ui->rbUseFile->setChecked(true);
    if(!keys.contains(FILEPATH))
        set.setValue(FILEPATH, ui->lbFilePath->text());
    else
        ui->lbFilePath->setText(set.value(FILEPATH).toString());
    if(!keys.contains(ADDTS))
        set.setValue(ADDTS, ui->cbAddTS->isChecked());
    else
        ui->cbAddTS->setChecked(set.value(ADDTS).toBool());
    if(!keys.contains(SLOWSCAN))
        set.setValue(SLOWSCAN, ui->cbSlotScan->isChecked());
    else
        ui->cbSlotScan->setChecked(set.value(SLOWSCAN).toBool());
    set.endGroup();
}

void MainPage::saveConfig()
{
    QSettings set(SETFILENAME, QSettings::IniFormat);
    set.beginGroup(DEFGROUP);
    set.setValue(URL, ui->leURL->text());
    set.setValue(THMAX, ui->sbThMax->value());
    set.setValue(TIMEOUT, ui->sbTimeout->value());
    set.setValue(AUTOSTEP, ui->cbAutoStep->isChecked());
    set.setValue(USEURL, ui->rbUseUrl->isChecked());
    set.setValue(FILEPATH, ui->lbFilePath->text());
    set.setValue(ADDTS, ui->cbAddTS->isChecked());
    set.setValue(SLOWSCAN, ui->cbSlotScan->isChecked());
    set.endGroup();
}

void MainPage::closeEvent(QCloseEvent *e)
{
    QMessageBox::StandardButton r = QMessageBox::question(this, tr("提示"),
                                                          tr("确认退出？退出后结果将丢失。"));
    if(r != QMessageBox::Yes){
        e->ignore();
        return;
    }
}

void MainPage::on_btnRec_clicked()
{
    if(mt->isRunning()){
        QMessageBox::warning(
            this, tr("警告"), tr("请先停止"));
        return;
    }
    QMessageBox::StandardButton r = QMessageBox::question(this, tr("提示"),
                                                          tr("确认恢复？恢复后配置将丢失。"));
    if(r != QMessageBox::Yes){
        return;
    }
    ui->leURL->setText("http://192.168.1.1:12345/udp/239.49.0.{[1-255]}:{6000#[8000-9999]}");
    ui->progressBar->setValue(0);
    ui->lbProgress->setText("0/0");
    ui->sbThMax->setValue(8);
    ui->sbTimeout->setValue(500);
    ui->lbStatus->setText("");
    ui->lbStatus2->setText("--");
    ui->cbAutoStep->setChecked(false);
    ui->lbFilePath->clear();
    ui->rbUseUrl->setChecked(true);
    ui->tbLog->setText(tr("1.对于局域网使用软路由组播转单播地址 \
                        \n    a.由于软路由及udpxy性能问题，超时时间建议设置为10000（10s），即不做限制，否则会报连接数量不足；\
                        \n    b.并发线程数量应当根据udpxy最大连接数设置，受软路由性能限制，单纯将udpxy最大连接数修改为很大并没有意义，建议值：udpxy设置为20，软件中设置为8；\
                        \n    c.udpxy会在长时间多连接轮询的不定期崩溃，建议不要选择过多的地址。\
                        \n    d.一般同一个ip地址只会部署一个频道，可以勾选发现相同IP已有成功地址后自动跳过后续 \
                        \n\n2.地址栏支持最多三个范围输入，范围规则如下：\
                        \n    a.零散值：1#3表示：1、3；\
                        \n    b.范围值 [8-10]表示：8、9、10；[08-10]表示：08、09、10；\
                        \n    c.可混合使用：1#3#[5-7]表示1、3、5、6、7。"));
}


void MainPage::on_btnOpenFile_clicked()
{
    QString path = QFileDialog::getOpenFileName(
        this, "选择文件", "", tr("model file(*.txt)"));
    if(path.isEmpty())
        return;
    ui->lbFilePath->setText(path);
}

