#pragma once

#include <QAbstractItemModel>
#include <QString>

namespace Iptv::Export {

/**
 * @brief CSV格式导出器
 *        将数据模型导出为CSV文件
 */
class CsvExporter
{
public:
    /** @brief 导出为CSV文件
     *  @param path  输出文件路径
     *  @param model 数据模型
     *  @return 是否导出成功 */
    bool exportToFile(const QString &path, QAbstractItemModel *model);

    /** @brief 获取文件过滤器字符串 */
    QString fileFilter() const;
};

} // namespace Iptv::Export
