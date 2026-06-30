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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &configPath, const QString &dbPath,
                        QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void requestReconfigure();

private slots:
    // File import
    void on_btnSelectFile_clicked(bool checked = false);
    void on_btnRun_clicked(bool checked = false);

    // Scan file generation
    void on_btnCreateScanFile_clicked(bool checked = false);

    // Export
    void on_btnSaveM3u_clicked(bool checked = false);
    void on_btnSaveTxt_clicked(bool checked = false);
    void on_btnExp_clicked(bool checked = false);

    // Tab navigation
    void on_tabWidget_currentChanged(int index);

    // Stream checking
    void on_btnCheckStart_clicked(bool checked = false);
    void on_btnCheckNext_clicked(bool checked = false);
    void on_btnCheckPrev_clicked(bool checked = false);
    void on_btnCheckRecheck_clicked(bool checked = false);
    void on_btnCheckSubmit_clicked(bool checked = false);
    void on_btnCheckRemove_clicked(bool checked = false);
    void on_cbCheckAddress_currentIndexChanged(int index);

    // Reset
    void on_btnReset_clicked(bool checked = false);
    void on_btnReset2_clicked(bool checked = false);

    // Misc
    void on_btnInfo_clicked(bool checked = false);
    void on_btnOpenScan_clicked(bool checked = false);
    void on_cbRemoveRepeat_clicked(bool checked);

    // Thread callbacks
    void onStreamProgress(int percent);
    void onStreamSucceeded(const Iptv::Multimedia::StreamInfo &info,
                           const QJsonObject &original);
    void onStreamFailed(const QString &error);

    void onExportProgress(int percent);
    void onExportFinished();

    // Logo
    void onLogoReady(const QPixmap &pixmap);
    void onLogoFailed(const QString &error);

    // Table sorting
    void onMainTableHeaderDoubleClicked(int logicalIndex);
    void onChannelTableHeaderDoubleClicked(int logicalIndex);

    // Timer
    void onHideTimerTimeout();

private:
    void setupConnections();
    void initModels();
    void loadConfig();
    void saveConfig();
    void refreshMainTable();
    void refreshChannelModel();

    Export::ExportOptions buildExportOptions() const;

    ::Ui::MainWindow *ui;

    // Services
    Iptv::Core::AppConfig *m_config;
    Iptv::Database::DatabaseManager *m_dbManager;
    Iptv::Database::ChannelRepository *m_channelRepo;
    Iptv::Database::SourceRepository *m_sourceRepo;
    Iptv::Network::HttpClient *m_httpClient;
    Iptv::Network::LogoFetcher *m_logoFetcher;
    Iptv::Multimedia::StreamProbe *m_streamProbe;
    Iptv::Export::XlsxExporter *m_xlsxExporter;
    Iptv::Logic::ChannelService *m_channelService;
    Iptv::Logic::CheckService *m_checkService;

    // Models
    QSqlQueryModel *m_queryModel;
    QSqlTableModel *m_tableModel;

    // Stream check state
    QJsonArray m_checkList;
    int m_checkIndex = -1;
    bool m_checkDataChange = false;
    bool m_suppressComboSignal = false;
    QString m_currentCheckUrl;

    // UI state
    int m_lastChannelLogicalIndex = -1;
    int m_lastChannelOrder = 0;

    QTimer *m_hideTimer;
};

} // namespace Iptv::Ui
