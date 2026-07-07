#include "tvlist_builder.h"
#include "export/exporter.h"

#include <QJsonArray>
#include <QJsonObject>

namespace Iptv::Logic {

QJsonArray TvListBuilder::buildForGroup(const QJsonArray &sources,
                                        const Export::ExportOptions &opts)
{
    Q_UNUSED(opts)
    //目前直接返回原始数据，后续可添加过滤和排序
    return sources;
}

void TvListBuilder::insertSorted(QJsonArray &arr, const QJsonObject &obj)
{
    int channelId = obj.value("channel_id").toInt();
    
    //按channel_id查找插入位置
    int insertIndex = arr.size();
    for (int i = 0; i < arr.size(); ++i) {
        int existingId = arr[i].toObject().value("channel_id").toInt();
        if (channelId < existingId) {
            insertIndex = i;
            break;
        }
    }
    
    //重建数组实现有序插入
    QJsonArray newArr;
    for (int i = 0; i < insertIndex; ++i) {
        newArr.append(arr[i]);
    }
    newArr.append(obj);
    for (int i = insertIndex; i < arr.size(); ++i) {
        newArr.append(arr[i]);
    }
    
    arr = newArr;
}

} // namespace Iptv::Logic
