#pragma once

#include <QMainWindow>
#include <QStringList>

namespace Ui { class MainWindow; }

namespace Iptv::Logic {
class ScanService;
}

namespace Iptv::Ui {

class ScanResultModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnStart_clicked();
    void on_btnStop_clicked();
    void on_btnClear_clicked();
    void on_btnSave_clicked();
    void on_btnOpenFile_clicked();
    void on_btnRestore_clicked();

    void onScanResult(const QString &url, int width, int height, int fps);
    void onScanProgress(int pos, int count, int okCount, int errorCount);
    void onCurrentUrlChanged(const QString &url);
    void onScanFinished();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void loadConfig();
    void saveConfig();
    void setupConnections();

    ::Ui::MainWindow *ui;
    Logic::ScanService *m_scanService;
    ScanResultModel *m_resultModel;
    QStringList m_resultList;
};

} // namespace Iptv::Ui
