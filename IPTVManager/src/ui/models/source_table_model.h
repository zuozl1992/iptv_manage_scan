#pragma once

#include <QSqlTableModel>

namespace Iptv::Ui {

class SourceTableModel : public QSqlTableModel
{
    Q_OBJECT

public:
    explicit SourceTableModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());

    void refresh();
};

} // namespace Iptv::Ui
