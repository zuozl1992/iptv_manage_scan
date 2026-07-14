#pragma once

#include <QSqlTableModel>

namespace Iptv::Ui {

/**
 * @brief 信号源表格模型
 *        基于QSqlTableModel，提供tv_source_info表的数据展示
 */
class SourceTableModel : public QSqlTableModel
{
    Q_OBJECT

public:
    explicit SourceTableModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());

    /** @brief 刷新模型数据 */
    void refresh();
};

} // namespace Iptv::Ui
