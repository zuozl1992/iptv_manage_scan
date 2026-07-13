#pragma once

#include <QString>

namespace Iptv::Core::ConfigKeys {

// ==================== 扫描设置 ====================
constexpr const char* SCAN_URL = "set/url";             ///< 扫描URL模板
constexpr const char* THREAD_MAX = "set/thread_max";    ///< 最大并发线程数
constexpr const char* TIMEOUT = "set/time_out";         ///< 探测超时时间（毫秒）
constexpr const char* AUTO_STEP = "set/auto_step";      ///< 跳过已有成功IP
constexpr const char* USE_URL = "set/use_url";          ///< true=URL输入, false=文件
constexpr const char* MODEL_FILE_PATH = "set/modelfile_path";   ///< 模型文件路径
constexpr const char* ADD_TS = "set/add_ts";            ///< 前缀"待整理 TS"
constexpr const char* SLOW_SCAN = "set/slow_scan";      ///< EIO时减半超时重试

// ==================== 默认值 ====================
constexpr int DEFAULT_THREAD_MAX = 8;           ///< 默认并发线程数
constexpr int DEFAULT_TIMEOUT = 500;            ///< 默认超时时间（毫秒）
constexpr bool DEFAULT_AUTO_STEP = false;       ///< 默认不跳过
constexpr bool DEFAULT_USE_URL = true;          ///< 默认URL输入模式
constexpr bool DEFAULT_ADD_TS = false;          ///< 默认不添加TS前缀
constexpr bool DEFAULT_SLOW_SCAN = false;       ///< 默认不启用慢速扫描

} // namespace Iptv::Core::ConfigKeys
