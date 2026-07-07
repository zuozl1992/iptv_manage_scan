#pragma once

#include <QString>
#include <QDateTime>

namespace Iptv::Database {

/**
 * @brief 电视频道信息结构体
 *        存储频道的基本信息，对应tv_info表
 */
struct TvChannel {
    int id = -1;            ///< 数据库主键ID
    int channelId = 0;      ///< 频道编号
    QString name;           ///< 频道名称
    QString group;          ///< 所属分组（央视/卫视/付费等）
    QString city;           ///< 所属城市
    QString describe;       ///< 频道描述
    QString notes;          ///< 备注
    QString logoName;       ///< 台标文件名
};

/**
 * @brief 电视信号源信息结构体
 *        存储频道的流媒体源信息，对应tv_source_info表
 */
struct TvSource {
    int id = -1;                ///< 数据库主键ID
    int tvId = -1;              ///< 关联的频道ID
    QString ip;                 ///< 流媒体源IP地址
    int port = 0;               ///< 端口号
    int width = 0;              ///< 视频宽度
    int height = 0;             ///< 视频高度
    int fps = 0;                ///< 帧率
    QDateTime lastCheckDate;    ///< 最后检测时间
    int type = 0;               ///< 源类型（source_type.id）
    QString notes;              ///< 备注
};

/**
 * @brief 信号源类型结构体
 *        定义不同画质等级，对应source_type表
 */
struct SourceType {
    int id = 0;         ///< 类型ID
    QString name;       ///< 类型名称（TS/SD/HD/4K/8K）
};

} // namespace Iptv::Database
