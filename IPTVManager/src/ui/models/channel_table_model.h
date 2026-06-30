#pragma once

#include <QSqlQueryModel>
#include <QString>

namespace Iptv::Ui {

class ChannelTableModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit ChannelTableModel(QObject *parent = nullptr);

    void refresh();
    void sortById();
    void sortByIp();
    void sortByName();
    void sortByWidth();
    void sortByHeight();
    void sortByFps();
    void sortByType();

private:
    void executeQuery(const QString &orderBy);
};

} // namespace Iptv::Ui
