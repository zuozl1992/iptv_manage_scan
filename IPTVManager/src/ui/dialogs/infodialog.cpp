#include "infodialog.h"
#include "ui_infodialog.h"

namespace Iptv::Ui {

InfoDialog::InfoDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new ::Ui::InfoDialog)
{
    ui->setupUi(this);
    setWindowTitle("说明");
}

InfoDialog::~InfoDialog()
{
    delete ui;
}

} // namespace Iptv::Ui
