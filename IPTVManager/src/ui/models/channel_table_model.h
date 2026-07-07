#pragma once

#include <QSqlQueryModel>
#include <QString>

namespace Iptv::Ui {

/**
 * @brief 频道表格模型
 *        基于QSqlQueryModel，提供频道信息的多维度排序展示
 */
class ChannelTableModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit ChannelTableModel(QObject *parent = nullptr);

    /** @brief 刷新模型数据 */
    void refresh();

    // ==================== 排序方法 ====================
    void sortById();        ///< 按频道编号排序
    void sortByIp();        ///< 按IP地址排序
    void sortByName();      ///< 按频道名称排序
    void sortByWidth();     ///< 按视频宽度排序
    void sortByHeight();    ///< 按视频高度排序
    void sortByFps();       ///< 按帧率排序
    void sortByType();      ///< 按源类型排序

private:
    /** @brief 执行排序查询
     *  @param orderBy SQL排序子句 */
    void executeQuery(const QString &orderBy);
};

} // namespace Iptv::Ui
