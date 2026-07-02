#include "core/config/appconfig.h"
#include "core/logging/logmanager.h"
#include "ui/mainwindow.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QStandardPaths>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("IPTVScanner");
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("IPTVScanner");

    // 加载翻译
    QTranslator *translator = new QTranslator(&app);
    bool loaded = false;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "IPTVScanner_" + QLocale(locale).name();
        if (translator->load(":/i18n/" + baseName)) {
            loaded = true;
            break;
        }
    }
    if (!loaded) {
        translator->load(":/i18n/IPTVScanner_zh_CN");
    }
    app.installTranslator(translator);

    // 初始化日志
    Iptv::Core::LogManager::init();
    qInfo() << "Application starting...";

    // 初始化配置
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configDir);
    QString configPath = configDir + "/IPTVScanner.ini";
    Iptv::Core::AppConfig::instance()->init(configPath);
    qInfo() << "Config loaded from:" << configPath;

    Iptv::Ui::MainWindow mainWindow;
    mainWindow.show();

    qInfo() << "Application started.";
    return app.exec();
}
