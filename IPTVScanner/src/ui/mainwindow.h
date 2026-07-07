#pragma once

#include <QMainWindow>
#include <QStringList>

namespace Ui { class MainWindow; }

namespace Iptv::Logic {
class ScanService;
}

namespace Iptv::Ui {

class ScanResultModel;

/**
 * @brief 扫描器主窗口
 *        提供URL输入、扫描控制、结果展示和日志输出功能
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnStart_clicked();     ///< 开始扫描
    void on_btnStop_clicked();      ///< 停止扫描
    void on_btnClear_clicked();     ///< 清空结果
    void on_btnSave_clicked();      ///< 保存结果到文件
    void on_btnOpenFile_clicked();  ///< 打开URL模板文件
    void on_btnRestore_clicked();   ///< 恢复默认配置

    void onScanResult(const QString &url, int width, int height, int fps);  ///< 扫描到一个结果
    void onScanProgress(int pos, int count, int okCount, int errorCount);   ///< 扫描进度更新
    void onCurrentUrlChanged(const QString &url);   ///< 当前扫描URL变化
    void onScanFinished();  ///< 扫描完成

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void loadConfig();          ///< 加载配置
    void saveConfig();          ///< 保存配置
    void setupConnections();    ///< 初始化信号槽连接

    ::Ui::MainWindow *ui;
    Logic::ScanService *m_scanService;      ///< 扫描服务
    ScanResultModel *m_resultModel;         ///< 结果表格模型
    QStringList m_resultList;               ///< 结果文本列表
};

} // namespace Iptv::Ui
