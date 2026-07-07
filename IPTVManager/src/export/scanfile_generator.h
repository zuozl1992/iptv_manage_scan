#pragma once

#include "exporter.h"
#include <QString>
#include <QStringList>

namespace Iptv::Export {

/**
 * @brief 扫描文件生成器
 *        生成用于IPTVScanner扫描新频道源的配置文件
 */
class ScanFileGenerator
{
public:
    /** @brief 生成扫描文件
     *  @param path      输出文件路径
     *  @param testUrls  测试频道URL列表
     *  @param scanIps   待扫描的IP地址列表
     *  @param opts      导出选项
     *  @return 是否生成成功 */
    bool generate(const QString &path, const QStringList &testUrls,
                  const QStringList &scanIps, const ExportOptions &opts);
};

} // namespace Iptv::Export
