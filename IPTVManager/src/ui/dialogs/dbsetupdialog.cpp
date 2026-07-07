#include "dbsetupdialog.h"
#include "ui_dbsetupdialog.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>

namespace Iptv::Ui {

DbSetupDialog::DbSetupDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new ::Ui::DbSetupDialog)
{
    ui->setupUi(this);
    setWindowTitle("数据库设置");
    
    //连接按钮信号
    connect(ui->btnSelectConfig, &QPushButton::clicked, this, &DbSetupDialog::onSelectConfig);
    connect(ui->btnCreateConfig, &QPushButton::clicked, this, &DbSetupDialog::onCreateConfig);
    connect(ui->btnSelectDb, &QPushButton::clicked, this, &DbSetupDialog::onSelectDb);
    connect(ui->btnCreateDb, &QPushButton::clicked, this, &DbSetupDialog::onCreateDb);
    connect(ui->btnOk, &QPushButton::clicked, this, &DbSetupDialog::onAccept);
}

DbSetupDialog::~DbSetupDialog()
{
    delete ui;
}

QString DbSetupDialog::configPath() const
{
    return m_configPath;
}

QString DbSetupDialog::dbPath() const
{
    return m_dbPath;
}

void DbSetupDialog::onSelectConfig(bool checked)
{
    Q_UNUSED(checked)
    QString path = QFileDialog::getOpenFileName(this, tr("选择配置文件"), "",
                                                tr("INI文件(*.ini)"));
    qDebug() << path;
    if (!path.isEmpty()) {
        m_configPath = path;
        ui->leConfigPath->setText(path);
    }
}

void DbSetupDialog::onCreateConfig(bool checked)
{
    Q_UNUSED(checked)
    QString path = QFileDialog::getSaveFileName(this, tr("创建配置文件"), {},
                                                tr("INI文件(*.ini)"));
    if (!path.isEmpty()) {
        m_configPath = path;
        ui->leConfigPath->setText(path);
    }
}

void DbSetupDialog::onSelectDb(bool checked)
{
    Q_UNUSED(checked)
    QString path = QFileDialog::getOpenFileName(this, tr("选择数据库文件"), {},
                                                tr("SQLite数据库(*.db)"));
    if (!path.isEmpty()) {
        m_dbPath = path;
        ui->leDbPath->setText(path);
    }
}

void DbSetupDialog::onCreateDb(bool checked)
{
    Q_UNUSED(checked)
    QString path = QFileDialog::getSaveFileName(this, tr("创建数据库文件"), {},
                                                tr("SQLite数据库(*.db)"));
    if (!path.isEmpty()) {
        m_dbPath = path;
        ui->leDbPath->setText(path);
        
        //复制默认数据库到指定路径
        QFile::copy(":/db_def.db", path);
        QFile::setPermissions(path, QFile::ReadOwner | QFile::WriteOwner);
    }
}

void DbSetupDialog::onAccept(bool checked)
{
    Q_UNUSED(checked)
    m_configPath = ui->leConfigPath->text();
    m_dbPath = ui->leDbPath->text();
    
    if (m_configPath.isEmpty() || m_dbPath.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请设置配置文件和数据库文件路径"));
        return;
    }
    
    emit setupCompleted();
}

} // namespace Iptv::Ui
