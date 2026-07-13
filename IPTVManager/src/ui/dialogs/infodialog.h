#pragma once

#include <QDialog>

namespace Ui { class InfoDialog; }

namespace Iptv::Ui {

/**
 * @brief 使用说明对话框
 *        显示应用的使用帮助信息
 */
class InfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InfoDialog(QWidget *parent = nullptr);
    ~InfoDialog();

private:
    ::Ui::InfoDialog *ui;
};

} // namespace Iptv::Ui
