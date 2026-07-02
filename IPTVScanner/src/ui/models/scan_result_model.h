#pragma once

#include "multimedia/stream_info.h"
#include <QAbstractTableModel>
#include <QVector>

namespace Iptv::Ui {

class ScanResultModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        ColUrl = 0,
        ColWidth,
        ColHeight,
        ColFps,
        ColSourceType,
        ColumnCount
    };

    explicit ScanResultModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void addResult(const Multimedia::StreamInfo &info);
    void clear();
    QVector<Multimedia::StreamInfo> results() const;

private:
    QVector<Multimedia::StreamInfo> m_results;
};

} // namespace Iptv::Ui
