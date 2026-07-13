#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QString>

namespace Iptv::Export {
struct ExportOptions;
}

namespace Iptv::Logic {

/**
 * @brief 电视频道列表构建器
 *        将原始信号源数据转换为可用于导出的频道列表格式
 */
class TvListBuilder
{
public:
    /** @brief 为指定分组构建频道列表
     *  @param sources 该分组下的原始信号源数据
     *  @param opts    导出选项
     *  @return 格式化后的频道列表JSON数组 */
    static QJsonArray buildForGroup(const QJsonArray &sources,
                                    const Export::ExportOptions &opts);

    /** @brief 按频道编号有序插入对象到数组
     *  @param arr 目标数组（会被修改）
     *  @param obj 待插入的对象 */
    static void insertSorted(QJsonArray &arr, const QJsonObject &obj);
};

} // namespace Iptv::Logic
