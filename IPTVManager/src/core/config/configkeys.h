#pragma once

#include <QString>

namespace Iptv::Core::ConfigKeys {

// ==================== 流媒体URL设置 ====================
constexpr const char* HTTP_URL = "set/http_url";        ///< HTTP流媒体服务器地址
constexpr const char* FILE_URL = "set/file_url";        ///< 文件服务器地址
constexpr const char* FCC_URL = "set/fcc_url";          ///< FCC服务器地址
constexpr const char* FCC_USE = "set/fcc_use";          ///< 是否启用FCC

// ==================== 组播/扫描设置 ====================
constexpr const char* GROUP_URL = "set/group_url";          ///< 组播地址模板
constexpr const char* GROUP_PORT = "set/group_port";        ///< 组播端口
constexpr const char* GROUP_TYPE = "set/group_type";        ///< 协议类型索引
constexpr const char* GROUP_ADD_TEST = "set/group_add_test";///< 是否包含测试频道

// ==================== 导出设置 ====================
constexpr const char* OUT_BY_ONE = "set/out_by_one";        ///< 是否按分组单独导出
constexpr const char* TV_GROUP_SELECT = "set/tv_group_select";///< 选中的频道分组
constexpr const char* TV_MERGE = "set/tv_merge";            ///< 是否合并相同频道
constexpr const char* OUT_ADD_LOGO = "set/out_add_logo";    ///< 是否添加台标
constexpr const char* OUT_ADD_HD = "set/out_add_hd";        ///< 是否添加高清后缀

// ==================== 默认值 ====================
constexpr const char* DEFAULT_HTTP_URL = "http://192.168.1.1:12345";   ///< 默认HTTP地址
constexpr const char* DEFAULT_FILE_URL = "http://192.168.1.1:54321";  ///< 默认文件地址
constexpr const char* DEFAULT_GROUP_URL = "239.49.{0#1#2}";           ///< 默认组播地址
constexpr const char* DEFAULT_GROUP_PORT = "6000#8000#8008";          ///< 默认端口

} // namespace Iptv::Core::ConfigKeys
