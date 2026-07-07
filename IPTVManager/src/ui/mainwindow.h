#pragma once

#include <QMainWindow>
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>

class QSqlQueryModel;
class QSqlTableModel;

namespace Ui { class MainWindow; }

namespace Iptv::Core {
class AppConfig;
}

namespace Iptv::Database {
class DatabaseManager;
class ChannelRepository;
class SourceRepository;
}

namespace Iptv::Network {
class HttpClient;
class LogoFetcher;
}

namespace Iptv::Multimedia {
class StreamProbe;
struct StreamInfo;
}

namespace Iptv::Export {
struct ExportOptions;
class XlsxExporter;
}

namespace Iptv::Logic {
class ChannelService;
class CheckService;
}

namespace Iptv::Ui {

/**
 * @brief 主窗口
 *        应用程序主界面，集成频道管理、信号源检测、文件导入导出等功能
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &configPath, const QString &dbPath,
                        QWidget *parent = nullptr);
    ~MainWindow();

signals:
    /** @brief 请求重新配置信号（切换数据库时触发） */
    void requestReconfigure();

private slots:
    // ---- 文件导入 ----
    void on_btnSelectFile_clicked(bool checked = false);    ///< 选择导入文件
    void on_btnRun_clicked(bool checked = false);           ///< 执行导入

    // ---- 扫描文件生成 ----
    void on_btnCreateScanFile_clicked(bool checked = false);    ///< 生成扫描文件

    // ---- 导出 ----
    void on_btnSaveM3u_clicked(bool checked = false);   ///< 导出M3U文件
    void on_btnSaveTxt_clicked(bool checked = false);   ///< 导出TXT文件
    void on_btnExp_clicked(bool checked = false);       ///< 导出Excel文件

    // ---- 页签切换 ----
    void on_tabWidget_currentChanged(int index);

    // ---- 流检测 ----
    void on_btnCheckStart_clicked(bool checked = false);    ///< 开始检测
    void on_btnCheckNext_clicked(bool checked = false);     ///< 下一个
    void on_btnCheckPrev_clicked(bool checked = false);     ///< 上一个
    void on_btnCheckRecheck_clicked(bool checked = false);  ///< 重新检测
    void on_btnCheckSubmit_clicked(bool checked = false);   ///< 提交修改
    void on_btnCheckRemove_clicked(bool checked = false);   ///< 删除信号源
    void on_cbCheckAddress_currentIndexChanged(int index);  ///< 切换信号源

    // ---- 重置 ----
    void on_btnReset_clicked(bool checked = false);     ///< 重置信号源数据
    void on_btnReset2_clicked(bool checked = false);    ///< 重置频道数据

    // ---- 其他 ----
    void on_btnInfo_clicked(bool checked = false);      ///< 显示帮助信息
    void on_btnOpenScan_clicked(bool checked = false);  ///< 打开扫描软件
    void on_cbRemoveRepeat_clicked(bool checked);       ///< 切换合并频道选项

    // ---- 异步回调 ----
    void onStreamProgress(int percent);                 ///< 流检测进度
    void onStreamSucceeded(const Iptv::Multimedia::StreamInfo &info,
                           const QJsonObject &original);///< 流检测成功
    void onStreamFailed(const QString &error);          ///< 流检测失败

    void onExportProgress(int percent);                 ///< 导出进度
    void onExportFinished();                            ///< 导出完成

    // ---- 台标 ----
    void onLogoReady(const QPixmap &pixmap);    ///< 台标加载成功
    void onLogoFailed(const QString &error);    ///< 台标加载失败

    // ---- 表格排序 ----
    void onMainTableHeaderDoubleClicked(int logicalIndex);      ///< 主表头双击排序
    void onChannelTableHeaderDoubleClicked(int logicalIndex);   ///< 频道表头双击排序

    // ---- 定时器 ----
    void onHideTimerTimeout();

private:
    void setupConnections();                ///< 初始化信号槽连接
    void initModels();                      ///< 初始化数据模型
    void loadConfig();                      ///< 加载配置
    void saveConfig();                      ///< 保存配置
    void refreshMainTable();                ///< 刷新主表格
    void refreshChannelModel();             ///< 刷新频道模型

    Export::ExportOptions buildExportOptions() const;   ///< 构建导出选项

    ::Ui::MainWindow *ui;

    // ==================== 服务组件 ====================
    Iptv::Core::AppConfig *m_config;                ///< 应用配置
    Iptv::Database::DatabaseManager *m_dbManager;   ///< 数据库管理器
    Iptv::Database::ChannelRepository *m_channelRepo;   ///< 频道数据仓库
    Iptv::Database::SourceRepository *m_sourceRepo;     ///< 信号源数据仓库
    Iptv::Network::HttpClient *m_httpClient;        ///< HTTP客户端
    Iptv::Network::LogoFetcher *m_logoFetcher;      ///< 台标获取器
    Iptv::Multimedia::StreamProbe *m_streamProbe;   ///< 流探测器
    Iptv::Export::XlsxExporter *m_xlsxExporter;     ///< Excel导出器
    Iptv::Logic::ChannelService *m_channelService;  ///< 频道业务服务
    Iptv::Logic::CheckService *m_checkService;      ///< 检测业务服务

    // ==================== 数据模型 ====================
    QSqlQueryModel *m_queryModel;       ///< 频道查询模型
    QSqlTableModel *m_tableModel;       ///< 信号源表格模型

    // ==================== 流检测状态 ====================
    QJsonArray m_checkList;             ///< 检测列表
    int m_checkIndex = -1;              ///< 当前检测索引
    bool m_checkDataChange = false;     ///< 检测数据是否变更
    bool m_suppressComboSignal = false; ///< 是否抑制下拉框信号
    QString m_currentCheckUrl;          ///< 当前检测URL

    // ==================== UI状态 ====================
    int m_lastChannelLogicalIndex = -1; ///< 频道表最后排序列
    int m_lastChannelOrder = 0;         ///< 频道表排序方向

    QTimer *m_hideTimer;                ///< 隐藏提示定时器
};

} // namespace Iptv::Ui
