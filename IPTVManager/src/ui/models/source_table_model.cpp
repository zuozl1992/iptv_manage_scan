#include "source_table_model.h"

namespace Iptv::Ui {

SourceTableModel::SourceTableModel(QObject *parent, QSqlDatabase db)
    : QSqlTableModel(parent, db)
{
    setTable("tv_info");
    setEditStrategy(QSqlTableModel::OnFieldChange);
    
    setHeaderData(0, Qt::Horizontal, "ID");
    setHeaderData(1, Qt::Horizontal, "频道");
    setHeaderData(2, Qt::Horizontal, "分组");
    setHeaderData(3, Qt::Horizontal, "城市");
    setHeaderData(4, Qt::Horizontal, "描述");
    setHeaderData(5, Qt::Horizontal, "备注");
    setHeaderData(6, Qt::Horizontal, "Logo");
}

void SourceTableModel::refresh()
{
    select();
}

} // namespace Iptv::Ui
