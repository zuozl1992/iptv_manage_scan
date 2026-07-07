#pragma once

#include <QAbstractItemModel>
#include <QObject>
#include <QString>

namespace Iptv::Export {

/**
 * @brief XLSX格式导出器
 *        将频道和信号源数据导出为Excel文件
 */
class XlsxExporter : public QObject
{
    Q_OBJECT

public:
    explicit XlsxExporter(QObject *parent = nullptr);

    /** @brief 导出为XLSX文件
     *  @param path       输出文件路径
     *  @param queryModel 频道数据模型
     *  @param tableModel 信号源数据模型
     *  @return 是否导出成功 */
    bool exportToFile(const QString &path,
                      QAbstractItemModel *queryModel,
                      QAbstractItemModel *tableModel);

    /** @brief 获取文件过滤器字符串 */
    QString fileFilter() const;

signals:
    /** @brief 导出进度变化信号
     *  @param percent 进度百分比（0-100） */
    void progressChanged(int percent);

private:
    /** @brief 写入单个工作表
     *  @param sheetName 工作表名称
     *  @param model     数据模型
     *  @param sheet     lxw_worksheet指针（void*避免头文件依赖）
     *  @return 是否写入成功 */
    bool writeSheet(const QString &sheetName, QAbstractItemModel *model, void *sheet);
};

} // namespace Iptv::Export
