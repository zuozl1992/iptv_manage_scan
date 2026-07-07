#include "channel_table_model.h"

namespace Iptv::Ui {

ChannelTableModel::ChannelTableModel(QObject *parent)
    : QSqlQueryModel(parent)
{
}

void ChannelTableModel::refresh()
{
    executeQuery("ORDER BY ti.channel_id");
}

void ChannelTableModel::sortById()
{
    executeQuery("ORDER BY ti.channel_id");
}

void ChannelTableModel::sortByIp()
{
    //按IP自然排序（使用json_extract实现四段IP的数值排序）
    executeQuery("ORDER BY json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[0]'), "
                 "json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[1]'), "
                 "json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[2]'), "
                 "json_extract('[' || replace(tsi.ip, '.', ',') || ']', '$[3]')");
}

void ChannelTableModel::sortByName()
{
    executeQuery("ORDER BY ti.name");
}

void ChannelTableModel::sortByWidth()
{
    executeQuery("ORDER BY tsi.width");
}

void ChannelTableModel::sortByHeight()
{
    executeQuery("ORDER BY tsi.height");
}

void ChannelTableModel::sortByFps()
{
    executeQuery("ORDER BY tsi.fps");
}

void ChannelTableModel::sortByType()
{
    executeQuery("ORDER BY tsi.type");
}

void ChannelTableModel::executeQuery(const QString &orderBy)
{
    //构建联表查询SQL
    QString sql = "SELECT ti.id, ti.name, ti.\"group\", tsi.ip, tsi.port, "
                  "tsi.width, tsi.height, tsi.fps, st.name as type, tsi.notes "
                  "FROM tv_info ti "
                  "JOIN tv_source_info tsi ON ti.id = tsi.tv_id "
                  "JOIN source_type st ON st.id = tsi.type ";
    
    if (!orderBy.isEmpty()) {
        sql += orderBy;
    }
    
    setQuery(sql);
    
    //设置中文列标题
    setHeaderData(0, Qt::Horizontal, "ID");
    setHeaderData(1, Qt::Horizontal, "频道");
    setHeaderData(2, Qt::Horizontal, "分组");
    setHeaderData(3, Qt::Horizontal, "地址");
    setHeaderData(4, Qt::Horizontal, "端口");
    setHeaderData(5, Qt::Horizontal, "宽");
    setHeaderData(6, Qt::Horizontal, "高");
    setHeaderData(7, Qt::Horizontal, "FPS");
    setHeaderData(8, Qt::Horizontal, "类型");
    setHeaderData(9, Qt::Horizontal, "备注");
}

} // namespace Iptv::Ui
