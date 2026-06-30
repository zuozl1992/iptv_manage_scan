#include "tvlist_builder.h"
#include "export/exporter.h"

#include <QJsonArray>
#include <QJsonObject>

namespace Iptv::Logic {

QJsonArray TvListBuilder::buildForGroup(const QJsonArray &sources,
                                        const Export::ExportOptions &opts)
{
    Q_UNUSED(opts)
    // For now, just return the sources as-is
    // Could add filtering, sorting, or transformation here
    return sources;
}

void TvListBuilder::insertSorted(QJsonArray &arr, const QJsonObject &obj)
{
    int channelId = obj.value("channel_id").toInt();
    
    // Find insertion point (sorted by channel_id)
    int insertIndex = arr.size();
    for (int i = 0; i < arr.size(); ++i) {
        int existingId = arr[i].toObject().value("channel_id").toInt();
        if (channelId < existingId) {
            insertIndex = i;
            break;
        }
    }
    
    // Insert at the correct position
    // QJsonArray doesn't have insert, so we need to rebuild
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
