#pragma once

#include "exporter.h"
#include <QHash>
#include <QJsonArray>
#include <QString>
#include <QStringList>

namespace Iptv::Export {

/**
 * @brief TXT格式导出器
 *        将频道列表导出为TXT播放列表文件
 */
class TxtExporter
{
public:
    /** @brief 导出为TXT文件（按默认分组顺序）
     *  @param path         输出文件路径
     *  @param groupedLists 按分组组织的频道数据
     *  @param opts         导出选项
     *  @return 是否导出成功 */
    bool exportToFile(const QString &path,
                      const QHash<QString, QJsonArray> &groupedLists,
                      const ExportOptions &opts);

    /** @brief 导出为TXT文件（按指定分组顺序）
     *  @param path         输出文件路径
     *  @param groupedLists 按分组组织的频道数据
     *  @param opts         导出选项
     *  @param groupOrder   分组输出顺序
     *  @return 是否导出成功 */
    bool exportToFile(const QString &path,
                      const QHash<QString, QJsonArray> &groupedLists,
                      const ExportOptions &opts,
                      const QStringList &groupOrder);

    /** @brief 获取文件过滤器字符串 */
    QString fileFilter() const;
};

} // namespace Iptv::Export
