#include "core/config/bootstrap.h"
#include "core/config/appconfig.h"
#include "core/logging/logmanager.h"
#include "ui/mainwindow.h"
#include "ui/dialogs/dbsetupdialog.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QStandardPaths>
#include <QDebug>
#include <QFileDialog>

/**
 * @brief 应用控制器
 *        管理配置对话框和主窗口的生命周期切换
 */
class AppController : public QObject
{
    Q_OBJECT
public:
    AppController(Iptv::Core::Bootstrap &bootstrap, QObject *parent = nullptr)
        : QObject(parent)
        , m_bootstrap(bootstrap)
        , m_mainWindow(nullptr)
    {
    }

    void showSetup()
    {
        m_setupWidget = new Iptv::Ui::DbSetupDialog();
        connect(m_setupWidget, &Iptv::Ui::DbSetupDialog::setupCompleted,
                this, &AppController::onSetupCompleted);
        m_setupWidget->show();
    }

public slots:
    void onReconfigure()
    {
        //销毁主窗口
        if (m_mainWindow) {
            m_mainWindow->deleteLater();
            m_mainWindow = nullptr;
        }

        //重新显示配置对话框
        showSetup();
    }

private slots:
    void onSetupCompleted()
    {
        QString configPath = m_setupWidget->configPath();
        QString dbPath = m_setupWidget->dbPath();

        //保存引导配置
        m_bootstrap.setConfigPath(configPath);
        m_bootstrap.setDbPath(dbPath);
        m_bootstrap.save();

        qInfo() << "Bootstrap saved: config=" << configPath << " db=" << dbPath;

        //创建并显示主窗口
        m_mainWindow = new Iptv::Ui::MainWindow(configPath, dbPath);
        m_mainWindow->setAttribute(Qt::WA_DeleteOnClose);
        connect(m_mainWindow, &Iptv::Ui::MainWindow::requestReconfigure,
                this, &AppController::onReconfigure);
        m_mainWindow->show();

        //隐藏配置对话框
        m_setupWidget->hide();

        qInfo() << "Application started.";
    }

private:
    Iptv::Core::Bootstrap &m_bootstrap;             ///< 引导配置
    Iptv::Ui::DbSetupDialog *m_setupWidget;         ///< 配置对话框
    Iptv::Ui::MainWindow *m_mainWindow;             ///< 主窗口
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("IPTVManager");
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("IPTVManager");
    
    //加载翻译文件
    QTranslator translator;
    QString locale = QLocale::system().name();
    if (translator.load(":/i18n/IPTVManager_" + locale)) {
        app.installTranslator(&translator);
    }
    
    //初始化日志系统
    Iptv::Core::LogManager::init();
    qInfo() << "Application starting...";
    
    //加载引导配置
    Iptv::Core::Bootstrap bootstrap;
    AppController *controller = new AppController(bootstrap, &app);
    
    if (bootstrap.load()) {
        QString configPath = bootstrap.configPath();
        QString dbPath = bootstrap.dbPath();
        qInfo() << "Bootstrap loaded: config=" << configPath << " db=" << dbPath;
        
        //直接创建并显示主窗口
        Iptv::Ui::MainWindow *mainWindow = new Iptv::Ui::MainWindow(configPath, dbPath);
        mainWindow->setAttribute(Qt::WA_DeleteOnClose);
        QObject::connect(mainWindow, &Iptv::Ui::MainWindow::requestReconfigure,
                         controller, &AppController::onReconfigure);
        mainWindow->show();
        
        qInfo() << "Application started with main window.";
        
        return app.exec();
    } else {
        //无引导配置，显示配置对话框
        qInfo() << "No bootstrap found, showing setup dialog...";
        controller->showSetup();
        
        qInfo() << "Setup dialog shown, entering event loop...";
        return app.exec();
    }
}

#include "main.moc"
