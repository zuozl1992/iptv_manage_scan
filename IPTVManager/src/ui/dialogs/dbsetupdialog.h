#pragma once

#include <QWidget>

namespace Ui { class DbSetupDialog; }

namespace Iptv::Ui {

/**
 * @brief 数据库配置对话框
 *        首次运行时引导用户选择或创建配置文件和数据库文件
 */
class DbSetupDialog : public QWidget
{
    Q_OBJECT

public:
    explicit DbSetupDialog(QWidget *parent = nullptr);
    ~DbSetupDialog();

    /** @brief 获取用户选择的配置文件路径 */
    QString configPath() const;

    /** @brief 获取用户选择的数据库文件路径 */
    QString dbPath() const;

signals:
    /** @brief 配置完成信号 */
    void setupCompleted();

private slots:
    void onSelectConfig(bool checked = false);   ///< 选择已有配置文件
    void onCreateConfig(bool checked = false);   ///< 创建新配置文件
    void onSelectDb(bool checked = false);       ///< 选择已有数据库
    void onCreateDb(bool checked = false);       ///< 创建新数据库
    void onAccept(bool checked = false);         ///< 确认配置

private:
    ::Ui::DbSetupDialog *ui;
    QString m_configPath;   ///< 配置文件路径
    QString m_dbPath;       ///< 数据库文件路径
};

} // namespace Iptv::Ui
