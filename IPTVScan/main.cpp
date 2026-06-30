#include "mainpage.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QStandardPaths>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "IPTVScan_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    QString customPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString temp;
    temp.append(argv[0]);
    int index = temp.lastIndexOf('/');
    if(index < 0)
        index = temp.lastIndexOf('\\');
    temp = temp.mid(index+1);
    customPath += '/';
    customPath += temp.split('.').at(0);
    customPath += ".ini";
    MainPage w(customPath);
    w.show();
    return a.exec();
}
