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
        // Delete main window
        if (m_mainWindow) {
            m_mainWindow->deleteLater();
            m_mainWindow = nullptr;
        }

        // Show setup dialog again
        showSetup();
    }

private slots:
    void onSetupCompleted()
    {
        QString configPath = m_setupWidget->configPath();
        QString dbPath = m_setupWidget->dbPath();

        // Save bootstrap
        m_bootstrap.setConfigPath(configPath);
        m_bootstrap.setDbPath(dbPath);
        m_bootstrap.save();

        qInfo() << "Bootstrap saved: config=" << configPath << " db=" << dbPath;

        // Create and show main window
        m_mainWindow = new Iptv::Ui::MainWindow(configPath, dbPath);
        m_mainWindow->setAttribute(Qt::WA_DeleteOnClose);
        connect(m_mainWindow, &Iptv::Ui::MainWindow::requestReconfigure,
                this, &AppController::onReconfigure);
        m_mainWindow->show();

        // Hide setup widget
        m_setupWidget->hide();

        qInfo() << "Application started.";
    }

private:
    Iptv::Core::Bootstrap &m_bootstrap;
    Iptv::Ui::DbSetupDialog *m_setupWidget;
    Iptv::Ui::MainWindow *m_mainWindow;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("IPTVManager");
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("IPTVManager");
    
    // Load translations
    QTranslator translator;
    QString locale = QLocale::system().name();
    if (translator.load(":/i18n/IPTVManager_" + locale)) {
        app.installTranslator(&translator);
    }
    
    // Initialize logging
    Iptv::Core::LogManager::init();
    qInfo() << "Application starting...";
    
    // Load bootstrap config
    Iptv::Core::Bootstrap bootstrap;
    AppController *controller = new AppController(bootstrap, &app);
    
    if (bootstrap.load()) {
        QString configPath = bootstrap.configPath();
        QString dbPath = bootstrap.dbPath();
        qInfo() << "Bootstrap loaded: config=" << configPath << " db=" << dbPath;
        
        // Create and show main window directly
        Iptv::Ui::MainWindow *mainWindow = new Iptv::Ui::MainWindow(configPath, dbPath);
        mainWindow->setAttribute(Qt::WA_DeleteOnClose);
        QObject::connect(mainWindow, &Iptv::Ui::MainWindow::requestReconfigure,
                         controller, &AppController::onReconfigure);
        mainWindow->show();
        
        qInfo() << "Application started with main window.";
        
        return app.exec();
    } else {
        qInfo() << "No bootstrap found, showing setup dialog...";
        controller->showSetup();
        
        qInfo() << "Setup dialog shown, entering event loop...";
        return app.exec();
    }
}

#include "main.moc"
