#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "models/scan_result_model.h"
#include "core/config/appconfig.h"
#include "logic/url_builder.h"
#include "logic/scan_service.h"

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>

namespace Iptv::Ui {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new ::Ui::MainWindow)
    , m_scanService(new Logic::ScanService(this))
    , m_resultModel(new ScanResultModel(this))
{
    ui->setupUi(this);
    setWindowTitle(tr("IPTV Scanner"));

    loadConfig();
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupConnections()
{
    connect(m_scanService, &Logic::ScanService::scanResult,
            this, [this](const Multimedia::StreamInfo &info) {
                m_resultModel->addResult(info);
                onScanResult(info.url, info.width, info.height, info.fps);
            });
    connect(m_scanService, &Logic::ScanService::scanProgress,
            this, &MainWindow::onScanProgress);
    connect(m_scanService, &Logic::ScanService::currentUrlChanged,
            this, &MainWindow::onCurrentUrlChanged);
    connect(m_scanService, &Logic::ScanService::scanFinished,
            this, &MainWindow::onScanFinished);
}

void MainWindow::onScanResult(const QString &url, int width, int height, int fps)
{
    QString log;
    if (ui->cbAddTS->isChecked())
        log = QString("待整理 TS %1x%2(%3),%4").arg(width).arg(height).arg(fps).arg(url);
    else
        log = QString("%1x%2(%3),%4").arg(width).arg(height).arg(fps).arg(url);
    m_resultList.append(log);
    ui->tbLog->append(log);
}

void MainWindow::onScanProgress(int pos, int count, int okCount, int errorCount)
{
    ui->lbProgress->setText(QString("%1/%2").arg(pos).arg(count));
    ui->progressBar->setValue(pos * 100 / count);
    ui->lbStatus2->setText(tr("成功：%1，失败：%2").arg(okCount).arg(errorCount));
}

void MainWindow::onCurrentUrlChanged(const QString &url)
{
    ui->lbStatus->setText(url);
}

void MainWindow::onScanFinished()
{
    ui->btnStart->setEnabled(true);
    ui->lbStatus->setText(tr("扫描完成"));
}

void MainWindow::on_btnStart_clicked()
{
    QMessageBox::StandardButton r = QMessageBox::question(this, tr("提示"), tr("确认开始？开始后将清空上次结果。"));
    if (r != QMessageBox::Yes)
        return;

    QStringList urls;
    bool ok = false;
    if (ui->rbUseUrl->isChecked())
        ok = !Logic::UrlBuilder::expand(ui->leURL->text()).isEmpty();
    else
        ok = !Logic::UrlBuilder::expandFromFile(ui->leURL->text()).isEmpty();

    if (!ok) {
        QMessageBox::warning(this, tr("警告"), tr("输入有误"));
        return;
    }

    if (ui->rbUseUrl->isChecked())
        urls = Logic::UrlBuilder::expand(ui->leURL->text());
    else
        urls = Logic::UrlBuilder::expandFromFile(ui->leURL->text());

    ui->tbLog->clear();
    m_resultModel->clear();
    m_resultList.clear();

    m_scanService->setUrlList(urls);
    m_scanService->setTimeout(ui->sbTimeout->value());
    m_scanService->setThreadMax(ui->sbThMax->value());
    m_scanService->setAutoStep(ui->cbAutoStep->isChecked());
    m_scanService->setSlowScan(ui->cbSlowScan->isChecked());

    ui->lbStatus->setText("");
    ui->lbStatus2->setText("--");
    ui->progressBar->setValue(0);
    ui->lbProgress->setText(QString("0/%1").arg(urls.length()));
    ui->btnStart->setEnabled(false);

    m_scanService->start();
}

void MainWindow::on_btnStop_clicked()
{
    m_scanService->stop();
}

void MainWindow::on_btnClear_clicked()
{
    ui->tbLog->clear();
    m_resultModel->clear();
    m_resultList.clear();
}

void MainWindow::on_btnSave_clicked()
{
    QString path = QFileDialog::getSaveFileName(
        this, tr("保存文件"), "", "txt(*.txt)");
    if (path.isEmpty())
        return;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;
    file.write(ui->tbLog->toPlainText().toLocal8Bit());
    file.close();
}

void MainWindow::on_btnOpenFile_clicked()
{
    QString path = QFileDialog::getOpenFileName(
        this, tr("选择文件"), "", tr("model file(*.txt)"));
    if (path.isEmpty())
        return;
    ui->leURL->setText(path);
}

void MainWindow::on_btnRestore_clicked()
{
    if (m_scanService->isRunning()) {
        QMessageBox::warning(this, tr("警告"), tr("请先停止"));
        return;
    }
    QMessageBox::StandardButton r = QMessageBox::question(this, tr("提示"),
                                                          tr("确认恢复？恢复后配置将丢失。"));
    if (r != QMessageBox::Yes)
        return;

    ui->leURL->setText("http://192.168.1.1:12345/udp/239.49.0.{[1-255]}:{6000#[8000-9999]}");
    ui->progressBar->setValue(0);
    ui->lbProgress->setText("0/0");
    ui->sbThMax->setValue(8);
    ui->sbTimeout->setValue(500);
    ui->lbStatus->setText("");
    ui->lbStatus2->setText("--");
    ui->cbAutoStep->setChecked(false);
    ui->cbAddTS->setChecked(false);
    ui->cbSlowScan->setChecked(false);
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
    saveConfig();
}

void MainWindow::loadConfig()
{
    Core::AppConfig *config = Core::AppConfig::instance();

    ui->leURL->setText(config->scanUrl());
    ui->sbThMax->setValue(config->threadMax());
    ui->sbTimeout->setValue(config->timeout());
    ui->cbAutoStep->setChecked(config->autoStep());
    ui->cbAddTS->setChecked(config->addTs());
    ui->cbSlowScan->setChecked(config->slowScan());

    if (config->useUrl())
        ui->rbUseUrl->setChecked(true);
    else
        ui->rbUseFile->setChecked(true);

    // Save config immediately to ensure defaults are written to disk on first run
    saveConfig();
}

void MainWindow::saveConfig()
{
    Core::AppConfig *config = Core::AppConfig::instance();

    config->setScanUrl(ui->leURL->text());
    config->setThreadMax(ui->sbThMax->value());
    config->setTimeout(ui->sbTimeout->value());
    config->setAutoStep(ui->cbAutoStep->isChecked());
    config->setAddTs(ui->cbAddTS->isChecked());
    config->setSlowScan(ui->cbSlowScan->isChecked());
    config->setUseUrl(ui->rbUseUrl->isChecked());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_scanService->isRunning()) {
        QMessageBox::StandardButton r = QMessageBox::question(this, tr("提示"),
                                                              tr("扫描正在进行中，确认退出？"));
        if (r != QMessageBox::Yes) {
            event->ignore();
            return;
        }
        m_scanService->stop();
    }

    QMessageBox::StandardButton r = QMessageBox::question(this, tr("提示"),
                                                          tr("确认退出？退出后结果将丢失。"));
    if (r != QMessageBox::Yes) {
        event->ignore();
        return;
    }

    saveConfig();
    event->accept();
}

} // namespace Iptv::Ui
