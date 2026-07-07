#pragma once

#include "exporter.h"
#include <QJsonArray>
#include <QString>
#include <QDir>

namespace Iptv::Export {

/**
 * @brief M3U格式导出器
 *        将频道列表导出为M3U播放列表文件
 */
class M3uExporter
{
public:
    /** @brief 导出为单个M3U文件
     *  @param path        输出文件路径
     *  @param channelList 频道数据列表
     *  @param opts        导出选项
     *  @return 是否导出成功 */
    bool exportToFile(const QString &path, const QJsonArray &channelList,
                      const ExportOptions &opts);

    /** @brief 按分组导出为多个M3U文件到目录
     *  @param dirPath     输出目录路径
     *  @param channelList 频道数据列表
     *  @param opts        导出选项
     *  @return 是否导出成功 */
    bool exportToDirectory(const QString &dirPath, const QJsonArray &channelList,
                           const ExportOptions &opts);

    /** @brief 获取文件过滤器字符串 */
    QString fileFilter() const;
};

} // namespace Iptv::Export
