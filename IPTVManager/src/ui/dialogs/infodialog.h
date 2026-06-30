#pragma once

#include <QDialog>

namespace Ui { class InfoDialog; }

namespace Iptv::Ui {

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
