#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <algorithm>

#include "core/config/appconfig.h"
#include "core/config/bootstrap.h"
#include "core/logging/logmanager.h"
#include "database/database_manager.h"
#include "database/channel_repository.h"
#include "database/source_repository.h"
#include "database/schema_manager.h"
#include "network/httpclient.h"
#include "network/logo_fetcher.h"
#include "multimedia/stream_probe.h"
#include "multimedia/ffmpeg_utils.h"
#include "export/m3u_exporter.h"
#include "export/txt_exporter.h"
#include "export/csv_exporter.h"
#include "export/xlsx_exporter.h"
#include "export/scanfile_generator.h"
#include "logic/channel_service.h"
#include "logic/check_service.h"
#include "logic/url_builder.h"
#include "dialogs/dbsetupdialog.h"
#include "dialogs/infodialog.h"
#include "styles/styleconstants.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QProcess>
#include <QDebug>
#include <QFont>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QDir>
#include <QCoreApplication>

//自定义委托：在下拉框中渲染富文本（加粗频道名）
class RichTextDelegate : public QStyledItemDelegate
{
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        
        painter->save();
        
        // Draw background
        if (opt.state & QStyle::State_Selected) {
            painter->fillRect(opt.rect, opt.palette.highlight());
            painter->setPen(opt.palette.highlightedText().color());
        } else {
            painter->setPen(opt.palette.text().color());
        }
        
        // Parse the text to make parts bold
        QString text = opt.text;
        QFont font = opt.font;
        
        // Find the bracket part [xxx] and channel name
        int bracketEnd = text.indexOf("] ");
        int separator1 = text.indexOf(" | ");
        int separator2 = text.indexOf(" | ", separator1 + 1);
        
        if (bracketEnd > 0 && separator1 > 0 && separator2 > 0) {
            QString bracket = text.left(bracketEnd + 1);  // [xxx]
            QString name = text.mid(bracketEnd + 2, separator1 - bracketEnd - 2);
            QString rest = text.mid(separator1);  //  | type | ip:port
            
            // Draw bracket part (bold)
            QFont boldFont = font;
            boldFont.setBold(true);
            QFontMetrics bfm(boldFont);
            int x = opt.rect.left() + 2;
            
            painter->setFont(boldFont);
            painter->drawText(x, opt.rect.top(), bfm.horizontalAdvance(bracket), opt.rect.height(),
                            Qt::AlignLeft | Qt::AlignVCenter, bracket);
            x += bfm.horizontalAdvance(bracket) + 2;
            
            // Draw name (bold)
            painter->drawText(x, opt.rect.top(), bfm.horizontalAdvance(name), opt.rect.height(),
                            Qt::AlignLeft | Qt::AlignVCenter, name);
            x += bfm.horizontalAdvance(name) + 2;
            
            // Draw rest (normal)
            QFontMetrics fm(font);
            painter->setFont(font);
            painter->drawText(x, opt.rect.top(), fm.horizontalAdvance(rest), opt.rect.height(),
                            Qt::AlignLeft | Qt::AlignVCenter, rest);
        } else {
            painter->drawText(opt.rect, Qt::AlignLeft | Qt::AlignVCenter, text);
        }
        
        painter->restore();
    }
    
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        QFontMetrics fm(opt.font);
        return QSize(fm.horizontalAdvance(opt.text) + 20, fm.height() + 4);
    }
};

namespace Iptv::Ui {

MainWindow::MainWindow(const QString &configPath, const QString &dbPath,
                       QWidget *parent)
    : QMainWindow(parent)
    , ui(new ::Ui::MainWindow)
    , m_hideTimer(new QTimer(this))
    , m_config(nullptr)
    , m_dbManager(nullptr)
    , m_channelRepo(nullptr)
    , m_sourceRepo(nullptr)
    , m_httpClient(nullptr)
    , m_logoFetcher(nullptr)
    , m_streamProbe(nullptr)
    , m_xlsxExporter(nullptr)
    , m_channelService(nullptr)
    , m_checkService(nullptr)
    , m_queryModel(nullptr)
    , m_tableModel(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("IPTV管理");
    resize(880, 650);
    
    //初始化日志系统
    Core::LogManager::init();
    
    //初始化配置管理
    m_config = Core::AppConfig::instance();
    m_config->init(configPath);
    
    //初始化数据库连接
    m_dbManager = new Database::DatabaseManager(dbPath, this);
    if (!m_dbManager->open()) {
        QMessageBox::critical(this, tr("错误"), tr("无法打开数据库"));
        return;
    }
    
    m_channelRepo = new Database::ChannelRepository(m_dbManager->database());
    m_sourceRepo = new Database::SourceRepository(m_dbManager->database());
    
    //初始化网络组件
    m_httpClient = new Network::HttpClient(this);
    m_logoFetcher = new Network::LogoFetcher(m_httpClient, this);
    
    //初始化多媒体组件
    m_streamProbe = new Multimedia::StreamProbe(this);
    
    //初始化导出组件
    m_xlsxExporter = new Export::XlsxExporter(this);
    
    //初始化业务服务
    m_channelService = new Logic::ChannelService(m_channelRepo, m_sourceRepo, this);
    m_checkService = new Logic::CheckService(m_streamProbe, m_sourceRepo, m_channelRepo, this);
    
    //初始化数据模型
    m_queryModel = m_sourceRepo->createJoinedTableModel(this);
    m_tableModel = new QSqlTableModel(this, *m_dbManager->database());
    m_tableModel->setTable("tv_info");
    m_tableModel->setSort(1, Qt::AscendingOrder);  //按频道ID排序
    m_tableModel->select();
    
    //初始化界面
    ui->pbExp->hide();
    ui->tabWidget->setCurrentIndex(0);
    
    initModels();
    loadConfig();
    setupConnections();
}

MainWindow::~MainWindow()
{
    saveConfig();
    delete ui;
}

void MainWindow::setupConnections()
{
    //表格表头双击排序
    connect(ui->tvMain->horizontalHeader(), &QHeaderView::sectionDoubleClicked,
            this, &MainWindow::onMainTableHeaderDoubleClicked);
    connect(ui->tvChannel->horizontalHeader(), &QHeaderView::sectionDoubleClicked,
            this, &MainWindow::onChannelTableHeaderDoubleClicked);
    
    //下拉框信号
    connect(ui->cbCheckAddress, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::on_cbCheckAddress_currentIndexChanged);
    
    //流探测信号
    connect(m_streamProbe, &Multimedia::StreamProbe::progressChanged,
            this, &MainWindow::onStreamProgress);
    connect(m_streamProbe, &Multimedia::StreamProbe::probeSucceeded,
            this, &MainWindow::onStreamSucceeded);
    connect(m_streamProbe, &Multimedia::StreamProbe::probeFailed,
            this, &MainWindow::onStreamFailed);
    
    //导出信号
    connect(m_xlsxExporter, &Export::XlsxExporter::progressChanged,
            this, &MainWindow::onExportProgress);
    
    //台标信号
    connect(m_logoFetcher, &Network::LogoFetcher::imageReady,
            this, &MainWindow::onLogoReady);
    connect(m_logoFetcher, &Network::LogoFetcher::fetchFailed,
            this, &MainWindow::onLogoFailed);
    
    //定时器
    connect(m_hideTimer, &QTimer::timeout, this, &MainWindow::onHideTimerTimeout);
}

void MainWindow::initModels()
{
    //配置主表格
    ui->tvMain->setModel(m_queryModel);
    ui->tvMain->setColumnHidden(0, true);
    ui->tvMain->horizontalHeader()->setStretchLastSection(true);
    
    //配置频道表格
    ui->tvChannel->setModel(m_tableModel);
    ui->tvChannel->setColumnHidden(0, true);
    ui->tvChannel->horizontalHeader()->setStretchLastSection(true);
    
    //设置频道表格中文列标题
    m_tableModel->setHeaderData(1, Qt::Horizontal, "频道ID");
    m_tableModel->setHeaderData(2, Qt::Horizontal, "名称");
    m_tableModel->setHeaderData(3, Qt::Horizontal, "分组");
    m_tableModel->setHeaderData(4, Qt::Horizontal, "城市");
    m_tableModel->setHeaderData(5, Qt::Horizontal, "描述");
    m_tableModel->setHeaderData(6, Qt::Horizontal, "备注");
    m_tableModel->setHeaderData(7, Qt::Horizontal, "Logo");
    
    //设置检测地址下拉框为等宽字体
    QFont monoFont("Monaco", 11);
    monoFont.setStyleHint(QFont::Monospace);
    ui->cbCheckAddress->setFont(monoFont);
    ui->cbCheckAddress->setItemDelegate(new RichTextDelegate(ui->cbCheckAddress));
}

void MainWindow::loadConfig()
{
    //从配置加载值到界面控件
    ui->leUrlFirst->setText(m_config->httpUrl());
    ui->leFileUrl->setText(m_config->fileUrl());
    ui->leGroupAddress->setText(m_config->groupAddress());
    ui->leGroupPort->setText(m_config->groupPort());
    ui->cbUrlType->setCurrentIndex(m_config->urlTypeIndex());
    ui->cbAddTest->setChecked(m_config->addTestEnabled());
    ui->cbRemoveRepeat->setChecked(m_config->mergeChannels());
    ui->cbOutAddLogo->setChecked(m_config->addLogoEnabled());
    ui->cbOutAddHD->setChecked(m_config->addHdSuffix());
    ui->leFCCUrl->setText(m_config->fccUrl());
    ui->cbUseFCC->setChecked(m_config->fccEnabled());
    
    //加载已选分组
    QStringList groups = m_config->selectedGroups();
    ui->cbYangshi->setChecked(groups.contains("央视"));
    ui->cbWeishi->setChecked(groups.contains("卫视"));
    ui->cbFufei->setChecked(groups.contains("付费"));
    ui->cbJiaoyu->setChecked(groups.contains("教育"));
    ui->cbDifang->setChecked(groups.contains("地方"));
    ui->cbGuoji->setChecked(groups.contains("国际"));
    ui->cbGuangbo->setChecked(groups.contains("广播"));
    ui->cbGouwu->setChecked(groups.contains("购物"));
    ui->cbLinshi->setChecked(groups.contains("临时"));
    ui->cbCeshi->setChecked(groups.contains("测试"));
}

void MainWindow::saveConfig()
{
    //保存界面配置值
    m_config->setHttpUrl(ui->leUrlFirst->text());
    m_config->setFileUrl(ui->leFileUrl->text());
    m_config->setGroupAddress(ui->leGroupAddress->text());
    m_config->setGroupPort(ui->leGroupPort->text());
    m_config->setUrlTypeIndex(ui->cbUrlType->currentIndex());
    m_config->setAddTestEnabled(ui->cbAddTest->isChecked());
    m_config->setMergeChannels(ui->cbRemoveRepeat->isChecked());
    m_config->setAddLogoEnabled(ui->cbOutAddLogo->isChecked());
    m_config->setAddHdSuffix(ui->cbOutAddHD->isChecked());
    m_config->setFccUrl(ui->leFCCUrl->text());
    m_config->setFccEnabled(ui->cbUseFCC->isChecked());
    
    //保存已选分组
    QStringList groups;
    if (ui->cbYangshi->isChecked()) groups << "央视";
    if (ui->cbWeishi->isChecked()) groups << "卫视";
    if (ui->cbFufei->isChecked()) groups << "付费";
    if (ui->cbJiaoyu->isChecked()) groups << "教育";
    if (ui->cbDifang->isChecked()) groups << "地方";
    if (ui->cbGuoji->isChecked()) groups << "国际";
    if (ui->cbGuangbo->isChecked()) groups << "广播";
    if (ui->cbGouwu->isChecked()) groups << "购物";
    if (ui->cbLinshi->isChecked()) groups << "临时";
    if (ui->cbCeshi->isChecked()) groups << "测试";
    m_config->setSelectedGroups(groups);
}

void MainWindow::refreshMainTable()
{
    m_queryModel->setQuery(
        "SELECT ti.id, ti.channel_id, ti.name, ti.\"group\", tsi.ip, tsi.port, "
        "tsi.width, tsi.height, tsi.fps, st.name as type, tsi.notes "
        "FROM tv_info ti "
        "JOIN tv_source_info tsi ON ti.id = tsi.tv_id "
        "JOIN source_type st ON st.id = tsi.type "
        "ORDER BY ti.channel_id, tsi.width DESC, tsi.fps DESC",
        *m_dbManager->database());
}

void MainWindow::refreshChannelModel()
{
    m_tableModel->select();
}

Export::ExportOptions MainWindow::buildExportOptions() const
{
    Export::ExportOptions opts;
    opts.urlPrefix = ui->leUrlFirst->text();
    opts.urlType = ui->cbUrlType->currentText();
    opts.fccUrl = ui->leFCCUrl->text();
    opts.fccEnabled = ui->cbUseFCC->isChecked();
    opts.addLogo = ui->cbOutAddLogo->isChecked();
    opts.addHdSuffix = ui->cbOutAddHD->isChecked();
    opts.mergeChannels = ui->cbRemoveRepeat->isChecked();
    opts.logoBaseUrl = ui->leFileUrl->text();
    
    // Get selected groups
    QStringList groups;
    if (ui->cbYangshi->isChecked()) groups << "央视";
    if (ui->cbWeishi->isChecked()) groups << "卫视";
    if (ui->cbFufei->isChecked()) groups << "付费";
    if (ui->cbJiaoyu->isChecked()) groups << "教育";
    if (ui->cbDifang->isChecked()) groups << "地方";
    if (ui->cbGuoji->isChecked()) groups << "国际";
    if (ui->cbGuangbo->isChecked()) groups << "广播";
    if (ui->cbGouwu->isChecked()) groups << "购物";
    if (ui->cbLinshi->isChecked()) groups << "临时";
    if (ui->cbCeshi->isChecked()) groups << "测试";
    opts.groups = groups;
    
    return opts;
}

// Slot implementations
void MainWindow::on_btnSelectFile_clicked(bool checked)
{
    Q_UNUSED(checked)
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择文件"), {},
                                                    tr("文本文件(*.txt)"));
    if (!filePath.isEmpty()) {
        ui->lbFilePath->setText(filePath);
    }
}

void MainWindow::on_btnRun_clicked(bool checked)
{
    Q_UNUSED(checked)
    QString filePath = ui->lbFilePath->text();
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先选择文件"));
        return;
    }
    
    int count = m_channelService->importFromTxt(filePath);
    if (count > 0) {
        QMessageBox::information(this, tr("成功"), tr("导入了 %1 条记录").arg(count));
        refreshMainTable();
        refreshChannelModel();
    } else {
        QMessageBox::warning(this, tr("警告"), tr("导入失败"));
    }
}

void MainWindow::on_btnCreateScanFile_clicked(bool checked)
{
    Q_UNUSED(checked)
    QString filePath = QFileDialog::getSaveFileName(this, tr("保存扫描文件"), {},
                                                    tr("文本文件(*.txt)"));
    if (filePath.isEmpty()) return;
    
    Export::ExportOptions opts = buildExportOptions();
    QStringList existingIps = m_sourceRepo->allIps();
    QStringList testUrls = m_sourceRepo->getTestUrls(ui->leGroupPort->text());
    QStringList scanIps = Logic::UrlBuilder::buildScanIps(
        ui->leGroupAddress->text(), ui->leGroupPort->text(), existingIps);
    
    Export::ScanFileGenerator generator;
    if (generator.generate(filePath, testUrls, scanIps, opts)) {
        QMessageBox::information(this, tr("成功"), tr("扫描文件已生成"));
    }
}

void MainWindow::on_btnSaveM3u_clicked(bool checked)
{
    Q_UNUSED(checked)
    QString filePath = QFileDialog::getSaveFileName(this, tr("保存M3U文件"), {},
                                                    tr("m3u(*.m3u)"));
    if (filePath.isEmpty()) return;
    
    Export::ExportOptions opts = buildExportOptions();
    QJsonArray channelList;
    
    // Build channel list from selected groups
    for (const QString &group : opts.groups) {
        QJsonArray list = m_sourceRepo->getTvListByGroup(group, true);
        for (const QJsonValue &val : list) {
            channelList.append(val);
        }
    }
    
    // Sort by channel_id
    QList<QJsonValue> sorted;
    for (const QJsonValue &val : channelList) {
        sorted.append(val);
    }
    std::sort(sorted.begin(), sorted.end(), [](const QJsonValue &a, const QJsonValue &b) {
        return a.toObject()["channel_id"].toInt() < b.toObject()["channel_id"].toInt();
    });
    QJsonArray sortedList;
    for (const QJsonValue &val : sorted) {
        sortedList.append(val);
    }
    
    Export::M3uExporter exporter;
    exporter.exportToFile(filePath, sortedList, opts);
    
    QMessageBox::information(this, tr("成功"), tr("M3U文件已导出"));
}

void MainWindow::on_btnSaveTxt_clicked(bool checked)
{
    Q_UNUSED(checked)
    QString filePath = QFileDialog::getSaveFileName(this, tr("保存TXT文件"), {},
                                                    tr("txt(*.txt)"));
    if (filePath.isEmpty()) return;
    
    Export::ExportOptions opts = buildExportOptions();
    QHash<QString, QJsonArray> groupedLists;
    
    for (const QString &group : opts.groups) {
        QJsonArray list = m_sourceRepo->getTvListByGroup(group, false);
        if (!list.isEmpty()) {
            groupedLists[group] = list;
        }
    }
    
    Export::TxtExporter exporter;
    exporter.exportToFile(filePath, groupedLists, opts, opts.groups);
    
    QMessageBox::information(this, tr("成功"), tr("TXT文件已导出"));
}

void MainWindow::on_btnExp_clicked(bool checked)
{
    Q_UNUSED(checked)
    QString filter;
#ifdef IPTV_OS_WINDOWS
    filter = "Excel(*.xlsx);;CSV(*.csv)";
#else
    filter = "CSV(*.csv)";
#endif
    
    QString filePath = QFileDialog::getSaveFileName(this, tr("导出"), {}, filter);
    if (filePath.isEmpty()) return;
    
    ui->pbExp->show();
    ui->pbExp->setValue(0);
    
    if (filePath.endsWith(".xlsx")) {
        m_xlsxExporter->exportToFile(filePath, m_queryModel, m_tableModel);
    } else {
        Export::CsvExporter exporter;
        exporter.exportToFile(filePath, m_queryModel);
    }
    
    m_hideTimer->start(3000);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index == 0) {
        refreshMainTable();
    } else if (index == 1) {
        refreshChannelModel();
    }
}

void MainWindow::on_btnCheckStart_clicked(bool checked)
{
    Q_UNUSED(checked)
    bool normalType = ui->rbNormal->isChecked();
    int order = ui->rbOrderByIP->isChecked() ? 0 : 1;
    
    //加载检测列表
    m_checkList = m_checkService->loadCheckList(normalType, order);
    m_checkIndex = 0;
    
    if (m_checkList.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("没有可检查的频道"));
        return;
    }
    
    //填充下拉框
    ui->cbCheckAddress->clear();
    for (int i = 0; i < m_checkList.size(); ++i) {
        QJsonObject obj = m_checkList[i].toObject();
        QString name = obj.value("name").toString();
        QString type = Multimedia::FfmpegUtils::sourceTypeName(obj.value("type").toInt());
        QString ip = obj.value("ip").toString();
        int port = obj.value("port").toInt();
        
        //格式：[序号] 频道名 | 类型 | IP:端口
        QString text = QString("[%1] %2 | %3 | %4:%5")
            .arg(i + 1, 3, 10, QChar(' '))
            .arg(name, -15)
            .arg(type, -4)
            .arg(ip)
            .arg(port);
        ui->cbCheckAddress->addItem(text);
    }
    
    ui->lbCheckCount->setText(QString("1 / %1").arg(m_checkList.size()));
    
    //开始第一个检测
    m_suppressComboSignal = true;
    ui->cbCheckAddress->setCurrentIndex(0);
    m_suppressComboSignal = false;
    
    QJsonObject source = m_checkList[0].toObject();
    QString url = QString("%1/%2/%3:%4")
        .arg(ui->leUrlFirst->text(),
             ui->cbUrlType->currentText(),
             source.value("ip").toString())
        .arg(source.value("port").toInt());
    
    if (ui->cbUseFCC->isChecked() && !ui->leFCCUrl->text().isEmpty()) {
        url += "?fcc=" + ui->leFCCUrl->text();
    }
    
    m_currentCheckUrl = url;
    m_checkService->startCheck(source, url);
}

void MainWindow::on_btnCheckNext_clicked(bool checked)
{
    Q_UNUSED(checked)
    if (m_checkIndex >= m_checkList.size() - 1) return;
    
    m_checkIndex++;
    m_suppressComboSignal = true;
    ui->cbCheckAddress->setCurrentIndex(m_checkIndex);
    m_suppressComboSignal = false;
    
    QJsonObject source = m_checkList[m_checkIndex].toObject();
    QString url = QString("%1/%2/%3:%4")
        .arg(ui->leUrlFirst->text(),
             ui->cbUrlType->currentText(),
             source.value("ip").toString())
        .arg(source.value("port").toInt());
    
    if (ui->cbUseFCC->isChecked() && !ui->leFCCUrl->text().isEmpty()) {
        url += "?fcc=" + ui->leFCCUrl->text();
    }
    
    m_currentCheckUrl = url;
    m_checkService->startCheck(source, url);
}

void MainWindow::on_btnCheckPrev_clicked(bool checked)
{
    Q_UNUSED(checked)
    if (m_checkIndex <= 0) return;
    
    m_checkIndex--;
    m_suppressComboSignal = true;
    ui->cbCheckAddress->setCurrentIndex(m_checkIndex);
    m_suppressComboSignal = false;
    
    QJsonObject source = m_checkList[m_checkIndex].toObject();
    QString url = QString("%1/%2/%3:%4")
        .arg(ui->leUrlFirst->text(),
             ui->cbUrlType->currentText(),
             source.value("ip").toString())
        .arg(source.value("port").toInt());
    
    if (ui->cbUseFCC->isChecked() && !ui->leFCCUrl->text().isEmpty()) {
        url += "?fcc=" + ui->leFCCUrl->text();
    }
    
    m_currentCheckUrl = url;
    m_checkService->startCheck(source, url);
}

void MainWindow::on_btnCheckRecheck_clicked(bool checked)
{
    Q_UNUSED(checked)
    if (m_checkIndex < 0 || m_checkIndex >= m_checkList.size()) return;
    
    QJsonObject source = m_checkList[m_checkIndex].toObject();
    QString url = QString("%1/%2/%3:%4")
        .arg(ui->leUrlFirst->text(),
             ui->cbUrlType->currentText(),
             source.value("ip").toString())
        .arg(source.value("port").toInt());
    
    if (ui->cbUseFCC->isChecked() && !ui->leFCCUrl->text().isEmpty()) {
        url += "?fcc=" + ui->leFCCUrl->text();
    }
    
    m_currentCheckUrl = url;
    m_checkService->startCheck(source, url);
}

void MainWindow::on_btnCheckSubmit_clicked(bool checked)
{
    Q_UNUSED(checked)
    if (m_checkIndex < 0 || m_checkIndex >= m_checkList.size()) {
        QMessageBox::warning(this, tr("警告"), tr("请先开始检查"));
        return;
    }
    
    QString name = ui->leCheckName->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请输入频道名称"));
        return;
    }
    
    QJsonObject source = m_checkList[m_checkIndex].toObject();
    
    // Get or create channel
    int tvId = m_channelService->getOrCreateChannel(name);
    if (tvId < 0) {
        QMessageBox::warning(this, tr("错误"), tr("无法创建频道"));
        return;
    }
    
    // Create source
    Database::TvSource tvSource;
    tvSource.tvId = tvId;
    tvSource.ip = source.value("ip").toString();
    tvSource.port = source.value("port").toInt();
    tvSource.width = ui->lbCheckWidth->text().toInt();
    tvSource.height = ui->lbCheckHeight->text().toInt();
    tvSource.fps = ui->lbCheckFps->text().toInt();
    tvSource.type = ui->cbCheckSourceType->currentIndex();
    tvSource.notes = ui->leCheckNotes->text();
    
    if (m_channelService->addSource(tvId, tvSource)) {
        m_checkDataChange = false;
        refreshMainTable();
        QMessageBox::information(this, tr("成功"), tr("频道源已更新"));
    } else {
        QMessageBox::warning(this, tr("错误"), tr("更新频道源失败"));
    }
}

void MainWindow::on_btnCheckRemove_clicked(bool checked)
{
    Q_UNUSED(checked)
    if (m_checkIndex < 0 || m_checkIndex >= m_checkList.size()) return;
    
    QJsonObject source = m_checkList[m_checkIndex].toObject();
    QString ip = source.value("ip").toString();
    
    if (QMessageBox::question(this, tr("确认"), tr("确定删除该源？")) == QMessageBox::Yes) {
        m_channelService->removeSource(ip);
        m_checkList.removeAt(m_checkIndex);
        
        if (m_checkIndex >= m_checkList.size()) {
            m_checkIndex = m_checkList.size() - 1;
        }
        
        refreshMainTable();
    }
}

void MainWindow::on_cbCheckAddress_currentIndexChanged(int index)
{
    if (m_suppressComboSignal) return;
    if (index < 0 || index >= m_checkList.size()) return;
    
    m_checkIndex = index;
    on_btnCheckRecheck_clicked();
}

void MainWindow::on_btnReset_clicked(bool checked)
{
    Q_UNUSED(checked)
    if (QMessageBox::question(this, tr("确认"), tr("确定要重新配置路径吗？")) == QMessageBox::Yes) {
        saveConfig();
        emit requestReconfigure();
        close();
    }
}

void MainWindow::on_btnReset2_clicked(bool checked)
{
    Q_UNUSED(checked)
    if (QMessageBox::question(this, tr("确认"), tr("确定恢复出厂设置？这将删除所有数据！")) == QMessageBox::Yes) {
        m_dbManager->resetToDefault();
        refreshMainTable();
        refreshChannelModel();
    }
}

void MainWindow::on_btnInfo_clicked(bool checked)
{
    Q_UNUSED(checked)
    InfoDialog dialog(this);
    dialog.exec();
}

void MainWindow::on_btnOpenScan_clicked(bool checked)
{
    Q_UNUSED(checked)
#ifdef Q_OS_WIN
    QProcess::startDetached("IPTVScan.exe");
#elif defined(Q_OS_MAC)
    // Try to find and open IPTVScan.app
    QStringList searchPaths = {
        "/Applications/IPTVScan.app",
        QDir::homePath() + "/Applications/IPTVScan.app",
        QCoreApplication::applicationDirPath() + "/../../IPTVScan.app"
    };
    
    QString appPath;
    for (const QString &path : searchPaths) {
        if (QDir(path).exists()) {
            appPath = path;
            break;
        }
    }
    
    if (appPath.isEmpty()) {
        // Use mdfind to search for IPTVScan.app
        QProcess process;
        process.start("mdfind", {"-name", "IPTVScan.app"});
        process.waitForFinished(5000);
        QString output = process.readAllStandardOutput().trimmed();
        if (!output.isEmpty()) {
            appPath = output.split("\n").first().trimmed();
        }
    }
    
    if (!appPath.isEmpty()) {
        QProcess::startDetached("open", {appPath});
    } else {
        QMessageBox::warning(this, tr("错误"), tr("未找到IPTVScan.app，请确认已安装"));
    }
#else
    QMessageBox::information(this, tr("提示"), tr("此功能仅在Windows和macOS上可用"));
#endif
}

void MainWindow::on_cbRemoveRepeat_clicked(bool checked)
{
    Q_UNUSED(checked)
    // Update UI based on merge mode
}

void MainWindow::onStreamProgress(int percent)
{
    ui->pbCheck->setValue(percent);
}

void MainWindow::onStreamSucceeded(const Multimedia::StreamInfo &info,
                                    const QJsonObject &original)
{
    //更新流信息到界面
    ui->lbCheckWidth->setText(QString::number(info.width));
    ui->lbCheckHeight->setText(QString::number(info.height));
    ui->lbCheckFps->setText(QString::number(info.fps));
    ui->cbCheckSourceType->setCurrentIndex(info.sourceType);
    ui->leCheckName->setText(original.value("name").toString());
    ui->leCheckNotes->setText(original.value("notes").toString());
    
    //更新视频预览
    if (info.frameData) {
        ui->openGLWidget->setData(info.frameData, info.width, info.height);
    }
    
    //更新序号标签
    ui->lbCheckCount->setText(QString("%1 / %2").arg(m_checkIndex + 1).arg(m_checkList.size()));
    
    //更新检测地址显示
    ui->lbCheckAddress->setPlainText(m_currentCheckUrl);
    
    //获取频道台标
    QString name = original.value("name").toString();
    if (!name.isEmpty()) {
        QString logoBaseUrl = ui->leFileUrl->text();
        if (!logoBaseUrl.isEmpty()) {
            QString logoUrl = logoBaseUrl + "/" + name + ".png";
            m_logoFetcher->fetch(QUrl(logoUrl));
        }
    }
}

void MainWindow::onStreamFailed(const QString &error)
{
    QMessageBox::warning(this, tr("错误"), error);
    ui->pbCheck->setValue(0);
}

void MainWindow::onLogoReady(const QPixmap &pixmap)
{
    ui->lbLogo->setPixmap(pixmap.scaled(ui->lbLogo->size(), 
                                        Qt::KeepAspectRatio, 
                                        Qt::SmoothTransformation));
}

void MainWindow::onLogoFailed(const QString &error)
{
    Q_UNUSED(error)
    ui->lbLogo->clear();
    ui->lbLogo->setText(tr("Logo:"));
}

void MainWindow::onExportProgress(int percent)
{
    ui->pbExp->setValue(percent);
}

void MainWindow::onExportFinished()
{
    ui->pbExp->setValue(100);
    m_hideTimer->start(3000);
}

void MainWindow::onMainTableHeaderDoubleClicked(int logicalIndex)
{
    //根据点击的列重新排序查询
    QString sql = "SELECT ti.id, ti.channel_id, ti.name, ti.\"group\", tsi.ip, tsi.port, "
                  "tsi.width, tsi.height, tsi.fps, st.name as type, tsi.notes "
                  "FROM tv_info ti "
                  "JOIN tv_source_info tsi ON ti.id = tsi.tv_id "
                  "JOIN source_type st ON st.id = tsi.type ";
    
    switch (logicalIndex) {
    case 1: //频道ID
        sql += "ORDER BY ti.channel_id, tsi.width DESC, tsi.fps DESC";
        break;
    case 2: //名称
        sql += "ORDER BY ti.name, tsi.width DESC, tsi.fps DESC";
        break;
    case 3: //分组
        sql += "ORDER BY ti.\"group\", ti.channel_id, tsi.width DESC, tsi.fps DESC";
        break;
    case 4: //IP地址
        sql += "ORDER BY json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[0]'),"
               "json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[1]'),"
               "json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[2]'),"
               "json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[3]'),"
               "tsi.width DESC, tsi.fps DESC";
        break;
    case 5: //端口
        sql += "ORDER BY tsi.port, tsi.width DESC, tsi.fps DESC";
        break;
    case 6: //宽度
        sql += "ORDER BY tsi.width DESC, ti.channel_id, tsi.fps DESC";
        break;
    case 7: //高度
        sql += "ORDER BY tsi.height DESC, ti.channel_id, tsi.fps DESC";
        break;
    case 8: //帧率
        sql += "ORDER BY tsi.fps DESC, ti.channel_id, tsi.width DESC";
        break;
    case 9: //类型
        sql += "ORDER BY tsi.type DESC, ti.channel_id, tsi.fps DESC";
        break;
    default:
        sql += "ORDER BY ti.channel_id, tsi.width DESC, tsi.fps DESC";
        break;
    }
    
    m_queryModel->setQuery(sql, *m_dbManager->database());
}

void MainWindow::onChannelTableHeaderDoubleClicked(int logicalIndex)
{
    //切换排序列或切换排序方向
    bool change = m_lastChannelLogicalIndex != logicalIndex;
    if (change)
        m_lastChannelOrder = 0;
    
    switch (logicalIndex) {
    case 1: //名称
        if (change)
            m_tableModel->setSort(1, Qt::AscendingOrder);
        else {
            if (m_lastChannelOrder == 0) {
                m_tableModel->setSort(1, Qt::DescendingOrder);
                m_lastChannelOrder = 1;
            } else {
                m_tableModel->setSort(1, Qt::AscendingOrder);
                m_lastChannelOrder = 0;
            }
        }
        break;
    case 2: //分组
        if (change)
            m_tableModel->setSort(2, Qt::AscendingOrder);
        else {
            if (m_lastChannelOrder == 0) {
                m_tableModel->setSort(2, Qt::DescendingOrder);
                m_lastChannelOrder = 1;
            } else {
                m_tableModel->setSort(2, Qt::AscendingOrder);
                m_lastChannelOrder = 0;
            }
        }
        break;
    case 3: //城市
        if (change)
            m_tableModel->setSort(3, Qt::AscendingOrder);
        else {
            if (m_lastChannelOrder == 0) {
                m_tableModel->setSort(3, Qt::DescendingOrder);
                m_lastChannelOrder = 1;
            } else {
                m_tableModel->setSort(3, Qt::AscendingOrder);
                m_lastChannelOrder = 0;
            }
        }
        break;
    default:
        break;
    }
    
    m_lastChannelLogicalIndex = logicalIndex;
    m_tableModel->select();
}

void MainWindow::onHideTimerTimeout()
{
    ui->pbExp->hide();
}

} // namespace Iptv::Ui
