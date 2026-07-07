#pragma once

#include <QString>
#include <QStringList>

namespace Iptv::Export {

/**
 * @brief 导出选项结构体
 *        封装频道列表导出时的所有可配置参数
 */
struct ExportOptions {
    QString urlPrefix;          ///< 流媒体服务器地址前缀
    QString urlType;            ///< 协议类型（udp/rtp）
    QString fccUrl;             ///< FCC服务器地址
    bool fccEnabled = false;    ///< 是否启用FCC
    bool addLogo = false;       ///< 是否添加台标
    bool addHdSuffix = false;   ///< 是否添加高清后缀
    bool mergeChannels = false; ///< 是否合并相同频道
    QString logoBaseUrl;        ///< 台标图片基础URL
    QStringList groups;         ///< 选中的频道分组列表
};

} // namespace Iptv::Export
